#ifndef 	__NAND_OAL__
#define  	__NAND_OAL__


#include <common.h>
//#include <asm/arch/dma.h>


//define the memory set interface
#define MEMSET(x,y,z)                   	memset(x,y,z)

//define the memory copy interface
#define MEMCPY(x,y,z)                   	memcpy(x,y,z)

//define the memory alocate interface
#define MALLOC(x)                       	malloc(x)

//define the memory release interface
#define FREE(x,size)                   		free(x)

//define the message print interface
#define PRINT							    printf
//#define PRINT(...)

#endif
