/*
*************************************************************************************
*                         			eBsp
*					   Operation System Adapter Layer
*
*				(c) Copyright 2006-2010, All winners Co,Ld.
*							All	Rights Reserved
*
* File Name 	: OSAL_Lib_C.h
*
* Author 		: javen
*
* Description 	: C库函数
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	   2010-09-07          1.0         	create this word
*		holi		   2010-12-03		   1.1			增加了OSAL_io_remap
*************************************************************************************
*/

#include "OSAL.h"

extern int kdb_trap_printk;

/* 普通内存分配 */
void * OSAL_malloc(__u32 Size)
{
	//return vmalloc(Size);
	/* to do */
	return 0;
}

void OSAL_free(void *pAddr)
{
    //vfree(pAddr);
    /* to do */
}

/* 连续的物理内存分配 */
void * OSAL_PhyAlloc(__u32 Size)
{
    void * addr;

    addr = malloc(Size);
	return addr;
}

void OSAL_PhyFree(void *pAddr, __u32 Size)
{
   free(pAddr);

   return;
}


/* 虚拟内存和物理内存之间的转化 */
unsigned int OSAL_VAtoPA(void *va)
{
	return (unsigned int)va;
    //return virt_to_phys(va);
}

void *OSAL_PAtoVA(unsigned int pa)
{
	return (void *)pa;
    //return phys_to_virt(pa);
}





int OSAL_putchar(int value)
{
	return 0;
}
int OSAL_puts(const char * value)
{
	return 0;
}
int OSAL_getchar(void)
{
	return 0;
}
char * OSAL_gets(char *value)
{
	return NULL;
}

//----------------------------------------------------------------
//  实用函数
//----------------------------------------------------------------
/* 字符串转长整形 */
long OSAL_strtol (const char *str, const char **err, int base)
{
	return 0;
}

/* 有符号十进制整形转字符串*/
void OSAL_int2str_dec(int input, char * str)
{
}

/* 十六进制整形转字符串*/
void OSAL_int2str_hex(int input, char * str, int hex_flag)
{
}

/* 无符号十进制整形转字符串*/
void OSAL_uint2str_dec(unsigned int input, char * str)
{
}



