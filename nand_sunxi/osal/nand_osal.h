#ifndef __NAND_OSAL_H__
#define __NAND_OSAL_H__

#include <common.h>

#define __OS_LINUX_SYSTEM__

#define NAND_IO_BASE_ADDR       0x01c03000

extern void *NAND_IORemap(unsigned int base_addr, unsigned int size);

//USE_SYS_CLK
extern int NAND_ClkRequest(void);
extern void NAND_ClkRelease(void);
extern int NAND_AHBEnable(void);
extern void NAND_AHBDisable(void);
extern int NAND_ClkEnable(void);
extern void NAND_ClkDisable(void);
extern int NAND_SetClk(unsigned int nand_clk);
extern int NAND_GetClk(void);

extern int NAND_RequestDMA(void);
extern int NAND_ReleaseDMA(void);
extern void NAND_DMAConfigStart(int rw, unsigned int buff_addr, int len);
extern int NAND_QueryDmaStat(void);
extern int NAND_WaitDmaFinish(void);

extern void NAND_PIORequest(void);
extern void NAND_PIORelease(void);

extern void NAND_EnRbInt(void);
extern void NAND_ClearRbInt(void);
extern int NAND_WaitRbReady(void);
extern void NAND_RbInterrupt(void);

extern void* NAND_Malloc(unsigned int Size);
extern void NAND_Free(void *pAddr, unsigned int Size);
extern int NAND_Print(const char * str, ...);

//define the memory set interface
#define MEMSET(x,y,z)            			memset((x),(y),(z))

//define the memory copy interface
#define MEMCPY(x,y,z)                   	memcpy((x),(y),(z))

//define the memory alocate interface
#define MALLOC(x)                       	NAND_Malloc((x))

//define the memory release interface
#define FREE(x,size)                    	NAND_Free((x),(size))

//define the message print interface
#define PRINT(...)							NAND_Print(__VA_ARGS__)	


#endif //__NAND_OSAL_H__