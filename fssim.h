#ifndef FSSIM_H
#define FSSIM_H
#define INODENUM 1024
#define BLOCKNUM (1UL<<18)
#define BLOCKSIZE 4096
#define SBSIZE 4096
#define INODESIZE 64
#define DIRITEMSIZE 32
#define DIRNUM (1UL<<7)
#define i2Inode(i) ((inode_t*)(inode+i*INODESIZE))
#define i2Block(i) ((block*)((blockdata+i*BLOCKSIZE)))

typedef struct SB
{
	char fsname[20]; 
}sb_t;

typedef union BLOCK
{
	char data[4096];
	int index[1024];
}block;

typedef struct INODE
{
	unsigned int flag; //设置最低位为有效位，次低位是文件种类
	unsigned int  size;
	int boc_data1; //如果是目录文件，只有该指针有效
	int boc_data2;
	int boc_data3;
	int boc_data4;
	int boc_data5;
}inode_t;

typedef struct DIR_ITEM
{
	char fileName[6*sizeof(int)];
	int inodeIndex;
	int valid;
}dir_item_t;

typedef char fsStart[SBSIZE+(BLOCKNUM<<3)+INODESIZE*INODENUM+BLOCKSIZE*BLOCKNUM];
#endif