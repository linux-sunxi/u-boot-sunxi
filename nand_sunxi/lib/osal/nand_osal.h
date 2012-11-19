#ifndef __NAND_OSAL_H__
#define __NAND_OSAL_H__

#include <common.h>

#define __OS_LINUX_SYSTEM__
#define __OS_USE_VADDR__

#define __OS_NAND_SUPPORT_RB_INT__
#ifdef __OS_NAND_SUPPORT_RB_INT__
    #define __OS_NAND_SUPPORT_INT__
#endif

#define __OS_NAND_SUPPORT_DMA_INT__
#ifdef __OS_NAND_SUPPORT_DMA_INT__
    #define __OS_NAND_SUPPORT_INT__
#endif    

#define __FPGA_TEST__

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

extern __s32 NAND_CleanFlushDCacheRegion(__u32 buff_addr, __u32 len);
extern __u32 NAND_DMASingleMap(__u32 rw, __u32 buff_addr, __u32 len);
extern __u32 NAND_DMASingleUnmap(__u32 rw, __u32 buff_addr, __u32 len);
extern __u32 NAND_VA_TO_PA(__u32 buff_addr);

extern void NAND_PIORequest(void);
extern void NAND_PIORelease(void);


extern void NAND_EnRbInt(void);
extern void NAND_ClearRbInt(void);
extern int NAND_WaitRbReady(void);
extern __s32 NAND_WaitDmaFinish(void);

extern void NAND_Memset(void* pAddr, unsigned char value, unsigned int len);
extern void NAND_Memcpy(void* pAddr_dst, void* pAddr_src, unsigned int len);
extern void* NAND_Malloc(unsigned int Size);
extern void NAND_Free(void *pAddr, unsigned int Size);
extern int NAND_Print(const char * str, ...);

extern __u32 NAND_GetIOBaseAddrCH0(void);
extern __u32 NAND_GetIOBaseAddrCH1(void);

#define NAND_IO_BASE_ADDR0		( NAND_GetIOBaseAddrCH0() )
#define NAND_IO_BASE_ADDR1		( NAND_GetIOBaseAddrCH1() )


//define the memory set interface
#define MEMSET(x,y,z)            			NAND_Memset((x),(y),(z))

//define the memory copy interface
#define MEMCPY(x,y,z)                   	NAND_Memcpy((x),(y),(z))

//define the memory alocate interface
#define MALLOC(x)                       	NAND_Malloc((x))

//define the memory release interface
#define FREE(x,size)                    	NAND_Free((x),(size))

//define the message print interface
#define PRINT(...)							NAND_Print(__VA_ARGS__)	





#endif //__NAND_OSAL_H__