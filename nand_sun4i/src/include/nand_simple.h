#ifndef __PHY_BOOT__
#define  __PHY_BOOT__

//#include "nand_oal.h"

//#define SUCESS	0
//#define FAIL	-1;
//#define BADBLOCK -2

struct boot_physical_param{
	unsigned char   chip; //chip no
	unsigned short  block; // block no within chip
	unsigned short  page; // apge no within block
	unsigned short  sectorbitmap; //done't care
	void   *mainbuf; //data buf
	void   *oobbuf; //oob buf
};

extern int PHY_SimpleErase(struct boot_physical_param * eraseop);
extern int PHY_SimpleRead(struct boot_physical_param * readop);
extern int PHY_SimpleWrite(struct boot_physical_param * writeop);

extern int PHY_SimpleWrite_1K(struct boot_physical_param * writeop);
extern int PHY_SimpleWrite_Seq(struct boot_physical_param * writeop);
extern int PHY_SimpleRead_Seq(struct boot_physical_param * readop);
extern int PHY_SimpleRead_1K(struct boot_physical_param * readop);

#endif
