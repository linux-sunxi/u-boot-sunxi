/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
#include <common.h>
#include <malloc.h>
#include "nand_uboot_fun.h"

extern int NAND_UbootInit(int boot_mode);
extern int NAND_UbootToPhy(void);
extern int NAND_UbootExit(void);
extern uint NAND_UbootRead(uint start, uint sectors, void *buffer);
extern uint NAND_UbootWrite(uint start, uint sectors, void *buffer);
extern int NAND_BurnBoot0(uint length, void *buffer);
extern int NAND_BurnUboot(uint length, void *buffer);
extern int NAND_PhyInit(void);
extern int NAND_PhyExit(void);
extern int NAND_Uboot_Erase(int erase_flag);

int nand_uboot_init(int boot_mode)
{
    return NAND_UbootInit(boot_mode);
}

int nand_uboot_exit(void)
{
    return NAND_UbootExit();
}

uint nand_uboot_read(uint start, uint sectors, void *buffer)
{
    int ret;

    ret = NAND_UbootRead(start, sectors, buffer);

    if(ret<0)
        return 0;
    else
        return sectors;
}

uint nand_uboot_write(uint start, uint sectors, void *buffer)
{
    int ret;

    ret = NAND_UbootWrite(start, sectors, buffer);

    if(ret<0)
        return 0;
    else
        return sectors;
}

int nand_download_boot0(uint length, void *buffer)
{
	int ret;

	if(!NAND_PhyInit())
	{
    	ret = NAND_BurnBoot0(length, buffer);
    }
    else
    {
    	ret = -1;
    }

    NAND_PhyExit();

    return ret;
}

int nand_download_uboot(uint length, void *buffer)
{
	int ret;

	debug("nand_download_uboot\n");
	if(!NAND_PhyInit())
	{
    	ret = NAND_BurnUboot(length, buffer);
    	debug("nand burn uboot error ret = %d\n", ret);
    }
    else
    {
    	debug("nand phyinit error\n");
    	ret = -1;
    }

    NAND_PhyExit();

    return ret;
}

int nand_uboot_erase(int user_erase)
{
    return NAND_Uboot_Erase(user_erase);
}


uint nand_uboot_get_flash_info(void *buffer, uint length)
{
    return NAND_GetParam(buffer, length);
}

uint nand_uboot_set_flash_info(void *buffer, uint length)
{
    return NAND_SetParam(buffer, length);
}

uint nand_uboot_get_flash_size(void)
{
    return NAND_GetDiskSize();
}


