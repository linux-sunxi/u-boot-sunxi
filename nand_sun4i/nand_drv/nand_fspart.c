/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON file system sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : fspart
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include <common.h>
#include <asm/arch/nand_fspart.h>
#include <asm/arch/nand_bsp.h>

typedef struct tag_CRC32_DATA
{
	unsigned int CRC;				//int的大小是32位
	unsigned int CRC_32_Tbl[256];	//用来保存码表
}CRC32_DATA_t;

char BOOTFSMBR_buf[1024] __attribute__ ((packed));
/*
************************************************************************************************************
*
*                                             _calc_crc32
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static __u32 _calc_crc32(void * buffer, __u32 length)
{
	__u32 i, j;
	CRC32_DATA_t crc32;		//
	__u32 CRC32 = 0xffffffff; //设置初始值

	crc32.CRC = 0;

	for( i = 0; i < 256; ++i)//用++i以提高效率
	{
		crc32.CRC = i;
		for( j = 0; j < 8 ; ++j)
		{
			//这个循环实际上就是用"计算法"来求取CRC的校验码
			if(crc32.CRC & 1)
				crc32.CRC = (crc32.CRC >> 1) ^ 0xEDB88320;
			else //0xEDB88320就是CRC-32多项表达式的值
				crc32.CRC >>= 1;
		}
		crc32.CRC_32_Tbl[i] = crc32.CRC;
	}

	CRC32 = 0xffffffff; //设置初始值
    for( i = 0; i < length; ++i)
    {
        CRC32 = crc32.CRC_32_Tbl[(CRC32^((unsigned char*)buffer)[i]) & 0xff] ^ (CRC32>>8);
    }

    //return CRC32;
	return CRC32^0xffffffff;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int FS_getpart_count(void)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;

	return mbr->PartCount;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int FS_getpart_start(int part_index)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;

	if(part_index >= mbr->PartCount)
	{
		return -1;
	}

	return mbr->array[part_index].addrlo;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int FS_getpart_capacity(int part_index)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;

	if(part_index >= mbr->PartCount)
	{
		return -1;
	}

	return mbr->array[part_index].lenlo;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int bootfs_scan_part(void)
{
   int i, part_index = 0;
   MBR    *mbr;

   NAND_LogicRead(0, MBR_SIZE >> 9 , BOOTFSMBR_buf);

   mbr = (MBR*)BOOTFSMBR_buf;
   if(!strncmp((const char*)mbr->magic, MBR_MAGIC, 8))
   {
        int crc = 0;

        crc = _calc_crc32(&mbr->version, MBR_SIZE-4);
        if(crc == mbr->crc32)
        {
            if(mbr->PartCount < 32)
            {
                PARTITION* pe = mbr->array;

                for(i=0; i<mbr->PartCount && i < MBR_MAX_PART_COUNT; i++, pe++)
                {
                    unsigned long long  len, addr;

                    addr = pe->addrhi;
                    addr = (addr << 32) | pe->addrlo;
                    len  = pe->lenhi;
                    len  = (len << 32) | pe->lenlo;

					if(!len)
					{
						len = NAND_GetDiskSize() - addr;
						pe->lenlo = len & 0xffffffff;
					}
                    part_index ++;
                }
            }
        }
   	}

	return part_index;
}



