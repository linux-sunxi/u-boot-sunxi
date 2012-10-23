/*
*******************************************************************************
*                                            Aone project ---- usb peripheral module
*
* File Name 	: encrypt.h
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2008.04.24
*
* Description 	:
*
* History 		:
*
********************************************************************************************************************
*/
#ifndef    __ENCRYPT_H__
#define    __ENCRYPT_H__

#include <config.h>
#include <common.h>

int  init_code(void);
uint  encode(void * ibuf, void * obuf, uint len);
uint  decode(void * ibuf, void * obuf, uint len);
int  exit_code(void);

#endif    //__ENCRYPT_H__
