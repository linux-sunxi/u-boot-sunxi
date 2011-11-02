/*
*********************************************************************************************************
*											        eBase
*
*
*
*						        (c) Copyright 2006-2010, AW China
*											All	Rights Reserved
*
* File    	: 	egon_drv_common_inc.h
* Date	:	2010-09-25
* By      	: 	holigun
* Version 	: 	V1.00
*********************************************************************************************************
*/
#ifndef	_NAND_MBR_H_
#define	_NAND_MBR_H_

#define     DOWNLOAD_MAP_NAME   "dlinfo.fex"
/* MBR       */
#define     MBR_SIZE			1024
#define   	MBR_MAGIC			"softw311"
#define     MBR_START_ADDRESS	0x00000000
#define     MBR_MAX_PART_COUNT	15
#define     MBR_COPY_NUM        4    //mbr的备份数量
#define     MBR_RESERVED        (MBR_SIZE - 20 - (MBR_MAX_PART_COUNT * sizeof(PARTITION)))   //mbr保留的空间

//分区信息, 64byte
typedef struct tag_PARTITION
{
	unsigned  int       addrhi;				//起始地址, 以扇区为单位
	unsigned  int       addrlo;				//
	unsigned  int       lenhi;				//长度
	unsigned  int       lenlo;				//
	unsigned  char      classname[12];		//次设备名
	unsigned  char      name[12];			//主设备名
	unsigned  int       user_type;          //用户类型
	unsigned  int       ro;                 //读写属性
	unsigned  char      res[16];			//保留
} __attribute__ ((packed))PARTITION;
//MBR信息
typedef struct tag_MBR
{
	unsigned  int       crc32;				        // crc 1k - 4
	unsigned  int       version;			        // 版本信息， 0x00000100
	unsigned  char 	    magic[8];			        //"softw311"
	unsigned  char 	    copy;				        //分数
	unsigned  char 	    index;				        //第几个MBR备份
	unsigned  short     PartCount;			        //分区个数
	PARTITION           array[MBR_MAX_PART_COUNT];	//
	unsigned  char      res[MBR_RESERVED];
}__attribute__ ((packed)) MBR;

typedef struct tag_one_part_info
{
	unsigned  char      classname[12];      //所烧写分区的次设备名
	unsigned  char      name[12];           //所烧写分区的主设备名
	unsigned  int       addrhi;             //所烧写分区的高地址，扇区单位
	unsigned  int       addrlo;             //所烧写分区的低地址，扇区单位
	unsigned  int       lenhi;				//所烧写分区的长度，高32位，扇区单位
	unsigned  int       lenlo;				//所烧写分区的长度，低32位，扇区单位
	unsigned  char      part_name[12];      //所烧写分区的名称，和MBR中的分区 classname 对应
	unsigned  char      dl_filename[16];    //所烧写分区的文件名称，长度固定16字节
	unsigned  char      vf_filename[16];    //所烧写分区的校验文件名称，长度固定16字节
	unsigned  int       encrypt;            //所烧写分区的数据是否进行加密 0:加密   1：不加密
}
dl_one_part_info;
//分区烧写信息
typedef struct tag_download_info
{
	unsigned  int       crc32;				        		//crc
	unsigned  int       version;                            //版本号  0x00000101
	unsigned  char 	    magic[8];			        		//"softw311"
	unsigned  int       download_count;             		//需要烧写的分区个数
	dl_one_part_info	one_part_info[MBR_MAX_PART_COUNT];	//烧写分区的信息
}
download_info;

#endif	//_NAND_MBR_H_

/* end of _NAND_MBR_H_ */

