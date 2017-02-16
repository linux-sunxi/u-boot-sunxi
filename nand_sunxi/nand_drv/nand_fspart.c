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
#include <fastboot.h>

typedef struct tag_CRC32_DATA
{
	unsigned int CRC;				//int的大小是32位
	unsigned int CRC_32_Tbl[256];	//用来保存码表
}CRC32_DATA_t;

char BOOTFSMBR_buf[1024];

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

int sunxi_nand_getpart_num(void)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;

	return mbr->PartCount;
}

int sunxi_nand_getpart_name(int index, char *buf)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;

	strncpy(buf, (const char *)mbr->array[index].name, 12);
	return 0;

}

int sunxi_nand_getpart_offset(int part_index)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;

	if(part_index >= mbr->PartCount)
	{
		return -1;
	}

	return mbr->array[part_index].addrlo;
}

int sunxi_nand_getpart_size(int part_index)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;

	if(part_index >= mbr->PartCount)
	{
		return -1;
	}

	return mbr->array[part_index].lenlo;
}

int sunxi_nand_getpart_offset_byname(const char *part_name)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;
	int			i;

	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, mbr->array[i].name))
		{
			return mbr->array[i].addrlo;
		}
	}

	return -1;
}

int sunxi_nand_getpart_size_byname(const char *part_name)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;
	int			i;

	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, mbr->array[i].name))
		{
			return mbr->array[i].lenlo;
		}
	}

	return -1;
}

/* get the partition info, offset and size
 * input: partition name
 * output: part_offset and part_size (in byte)
 */
int sunxi_nand_getpart_info_byname(const char *part_name, loff_t *part_offset, loff_t *part_size)
{
	MBR        *mbr  = (MBR*)BOOTFSMBR_buf;
	int			i;

	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)mbr->array[i].name))
		{
			*part_offset = mbr->array[i].addrlo * 512;
			*part_size = mbr->array[i].lenlo * 512;
			return 0;
		}
	}

	return -1;
}

int sunxi_nand_scan_partition(void)
{
   int i, part_index = 0;
   MBR    *mbr;

//	while((*(volatile __u32 *)0) != 0);
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
