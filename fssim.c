#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fssim.h"

fsStart ramfs;
void* superblock = &ramfs[0];
void* bitmap = &ramfs[SBSIZE];
void* inode = &ramfs[SBSIZE+(BLOCKNUM<<3)];
void* blockdata = &ramfs[SBSIZE+(BLOCKNUM<<3)+INODESIZE*INODENUM];

char menu[256] = "root";
char buf[4096];
int threshold = 1<<17;
int inodeOfCurDir=0;
inode_t* curInode=NULL;

int adddir(char* filename)
{
	dir_item_t *curdir= (dir_item_t*)i2Block(i2Inode(inodeOfCurDir)->boc_data1);
   	for (i = 0; i<DIRNUM; i++)
		{
			if (curdir[i].valid == 0)
			{
				int fid = getNewInode();
				inode_t* tmp = i2Inode(fid);
				tmp->flag = 3;
				strcpy(curdir[i].fileName,filename);
				curdir[i].inodeIndex=fid; //增加新条目
				curdir[i].valid = 1; 
				break;
			}
		}
	if(i==DIRNUM)
	{
		printf("dir number is full\n");
		return -1;
	}

}
int simWrite(int inode,int offset,int size)
{

}
int simRead(int inode,int offset,int size)
{
	
}

/*
filename必须是相对路径
ptr1->4kb
ptr2->4kb
ptr3->4MB
ptr4->4MB
ptr5->4kb->4MB
*/
int mycreate(char* filename,unsigned int size);
{
	if(size>4096*1024*(1024+2)+4096*2)
	{
		printf("file is too large\n");
		return -1;
	}
	int adddir(char* filename);
	if(adddir==-1)
		return -1;
	int fid ＝ getInode(filename);
	inode_t* inode = i2Inode(fid);
	inode->size = size;
	int* tmp = &inode->boc_data1;
	int small = size<<10>>10;
	int big = size>>22;
	if(size>4096*2)
	{
		small = 0;
		big++;
	}
	if(small>4096)
	{
		int blockid0 = getNewBlcok(0);
		int blockid1 = getNewBlcok(0);
		tmp[0] = blockid0;
		tmp[1] = blockid1;
	}
	else
	{
		int blockid0 = getNewBlcok(0);
		tmp[0]  = blockid0;
	}
	if(big>0)
	{
		int blockid = getNewInode(1);
		tmp[2] = blockid;
		if(big>1)
		{
			blockid = getNewBlcok(1);
			tmp[3] = blockid;
		}
		if(big>2)
		{
			int tmp = big-2;
			int h;
			blockid = getNewBlcok(0);
			tmp[4] = blockid;
			block* indexfile = i2Block(blockid);
			for(h=0;h<tmp;h++)
			{
				blockid = getNewBlcok(1);
				indexfile->index[h] = blockid;
			}
		}
	}
	return fid;
} 

int mywrite(char* filename,int offset,int size)
{
	int curnode = getInode(filename);
	if(curnode=-1)
	{
		curnode =  create(filename,offset+size);
	}
	if(simWrite(curnode,offset,size)==-1)
		return-1;
}
int myread(char* filename,int offset,int size)
{
	int curnode = getInode(filename);
	if(curnode = -1)
	{
		printf("No such file\n");
		return -1;
	}
	else
	{
		if(simRead(curnode,offset,size)==-1)
			return -1;
	}
	return 0;
}

int getNewInode()
{
	int i;
	for(i=0;i<1024;i++)
	{
		inode_t* tmp = (inode_t*)(inode+i*INODESIZE);
		if(tmp->flag==0)
		{
			tmp->flag = 1;
			return i;
		}
	}	
	printf("no free inode\n");
	return -1;
}

int  freeInode(int i)
{
	inode_t* tmp = (inode_t*)(inode+i*INODESIZE);
	tmp->flag = 0;
	return 0;
}

int getNewBlcok(int flag)
{
	if(flag==0)
	{
		int i;
		for(i=0;i<threshold;i++)
		{
			unsigned char *tmp = (unsigned char *)(bitmap+(i>>3));
			unsigned mask = i & 7UL;
			if((*tmp & (1UL<<(7-mask)))==0)
			{
				*tmp = (*tmp | (1UL<<(7-mask)));
				return i;
			}
		}
		for(i=threshold;i<threshold+1024;i=i+8)
		{
			unsigned char *tmp = (unsigned char*)(bitmap+(i>>3));
			if((*tmp)!=0)
				{
					printf("fail to move down threshold\n");
					return -1;
				}
		}
		if(i == threshold+1024)
		{	
			unsigned char* tmp =  (unsigned char *)(bitmap+(threshold>>3));
			*tmp = 128;
			threshold+=1024;
			return threshold-1024;
		}
	}
	else if(flag==1)
	{
		int i;
		for(i=BLOCKNUM-1024;i>=threshold;i=i-1024)
		{
			unsigned char* tmp = (unsigned char *)(bitmap+(i>>3));
			int j;
			for(j=0;j<128;j++)
			{
				if(tmp[j]!=0)
				{
					break;
				}
			}
			if(j==1024)
			{
				printf("find a big block\n");
				tmp = (unsigned char *)(bitmap+(i>>3));
				for(j=0;j<128;j++)
				{
					tmp[j]=255;
				}
				return i;
			}
		}
		for(i=threshold-1024;i<threshold;i=i+8)
		{
			unsigned char *tmp = (unsigned char*)(bitmap+(i>>3));
			if((*tmp)!=0)
				{
					printf("fail to move up threshold\n");
					break;
					return -1;
				}
		}
		if(i == threshold)
		{
			i=i-1024;
			printf("succed to move up threshold\n");
			unsigned char *tmp = (unsigned char *)(bitmap+(i>>3));
			int j;
			for(j=0;j<128;j++)
			{
				tmp[i]=255;
			}
			threshold-=1024;
			return i;	
		}
	}
	printf("flag is wrong\n");
	return -1;

}

int freeBlock(int i,int flag)
{
	if(flag==0)
	{
		unsigned char* tmp = (unsigned char *)(bitmap+(i>>3));
		unsigned mask = i & 7UL;
		if((*tmp & (1UL<<(7-mask)))==1)
		{
			*tmp = (*tmp & (~(1UL<<(7-mask))));
			return 0;
		}
		else
		{
			printf("trying to free a free block\n");
			return -1;
		}
	}
	else if(flag == 1)
	{
		unsigned char* tmp = (unsigned char *)(bitmap+(i>>3));
		int j;
		for(j=0;j<128;j++)
		{
			if(tmp[j]!=255)
			{
				printf("something wrong");
				return -1;
			}
			else
			{
				tmp[j]=255;
			}
		}
		return 0;
	}
	printf("flag is wrong\n");
	return -1;
}

// 通过路径名获取文件id
int getInode(const char *name)
{
	char filename[24]={0};
	int len = strlen(name);
	int curnode=inodeOfCurDir;
	int j=0,k=0;
	if(name[0]=='/')
	{
		curnode = 0;
		j=1;
	}
	for(;j<len;j++)
	{
		if(name[j]=='/')
		{
			if(strlen(filename)!=0)
			{
				dir_item_t *curdir= (dir_item_t*)i2Block(i2Inode(curnode)->boc_data1);
				int i;
				for (i = 0; i < DIRNUM; i++)
				{
					if ((curdir[i].valid == 1) &&
					strcmp(curdir[i].fileName, filename) == 0)
					{
						curnode = curdir[i].inodeIndex;
						break;
					}
				}
				if(i==DIRNUM)
				{
					printf("cann't find the dir");
				}
				memset(filename,0,24);
				k=0;
			}
		}
		else
		{
			filename[k++]=name[j]; 
		}
	}
	if(strlen(filename)!=0)
	{
		dir_item_t *curdir= (dir_item_t*)i2Block(i2Inode(curnode)->boc_data1);
		int i;
		for (i = 0; i < DIRNUM; i++)
		{
			printf("%d: %d %s\n",i,curdir[i].valid,curdir[i].fileName);
			if ((curdir[i].valid == 1) &&
			strcmp(curdir[i].fileName, filename) == 0)
			{
				curnode = curdir[i].inodeIndex;
				break;
			}
		}
		if(i==DIRNUM)
		{
			printf("cann't find the dir");
		}
		memset(filename,0,24);
		k=0;
	}
	return curnode;
}

//初始化根目录
int init()
{
	sb_t* tmp = (sb_t*)superblock;
	strcpy(tmp->fsname,"simplefilesystem");
	int blockid;
	if((blockid = getNewBlcok(0))!=0)
	{
		printf("wrong in func init\n");
		return -1;
	}
	inode_t* curInode = i2Inode(0);
	curInode->boc_data1=blockid; //初始化新目录inode
	curInode->flag = 1;
	return 0;
}


int main(int argc, char **argv)
{
   if(init() == -1)
   {
   		printf("init failure...\n");
   		return -1;
   }
   char cmd[32] = {0};
   while(1)
   {
   		printf("\n%s$",menu);
   		scanf("%s",cmd);
   		if(strcmp(cmd,"write")==0)
   		{
   			char filename[100];
   			int offset;
   			int size;
   			scanf("%s",filename);
   			scanf("%d",&offset);
   			scanf("%d",&size);
   			int fid = mywrite(filename,offset,size);
   			if(fid==-1)
   			{
   				printf("write failure!\n");
   				return -1;
   			}
   		}
   		else if(strcmp(cmd,"read")==0)
   		{
   			char filename[24];
   			int offset;
   			int size;
   			scanf("%s",filename);
   			scanf("%d",&offset);
   			scanf("%d",&size);
   			int fid = myread(filename,offset,size);
   		}
   		else if(strcmp(cmd,"mkdir")==0)
   		{
   			char filename[24];
   			int i;
   			scanf("%s",filename);
   			dir_item_t *curdir= (dir_item_t*)i2Block(i2Inode(inodeOfCurDir)->boc_data1);
   			for (i = 0; i<DIRNUM; i++)
				{
					if (curdir[i].valid == 0)
					{
						int fid = getNewInode();
						printf("fid: %d\n",fid);
						int blockid = getNewBlcok(0);
						printf("blockid: %d\n",blockid);
						strcpy(curdir[i].fileName,filename);
						curdir[i].inodeIndex=fid; //增加新条目
						curdir[i].valid = 1; 
						inode_t* tmp = i2Inode(fid);
						tmp->boc_data1=blockid; //初始化新目录inode
						tmp->flag = 1; 
						break;
					}
				}
			if(i==DIRNUM)
			{
				printf("dir number is full\n");
				return -1;
			}

   		}
   		else if(strcmp(cmd,"ls")==0)
   		{
   			int i;
   			dir_item_t *curdir= (dir_item_t*)i2Block(i2Inode(inodeOfCurDir)->boc_data1);
   			for (i = 0; i<DIRNUM; i++)
				{
					if (curdir[i].valid == 1)
					{
						printf("%s\n",curdir[i].fileName);
					}
				}
   		}
   		else if(strcmp(cmd,"cd")==0)
   		{
   			char filename[100];
   			scanf("%s",filename);
   			int fid = getInode(filename);
   			printf("in func cd ,fid: %d\n",fid);
   			inodeOfCurDir = fid;
   		}
   }
}