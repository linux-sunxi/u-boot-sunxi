//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//    I MMMMMMMMMMI                                                                                         //
//      I FF      BBI                                                                                       //
//      I FF      I EE  FFMMFFI MMMMLL  I MMMMMMEE          EEMMBBLLBBFF    FFMMMMBBI   I MMBBBBMMMMI       //
//      I FF        EEI   I BBBBI   I     I     LLFF      EELL  I BBFF    FFFF    I BBI   I BBI   I EE      //
//      I FF        EEI   I FF          I BBMMMMMMFF      FF      I FF  I EE        I LL  I FF      EEI     //
//      I FF      I EE    I FF          EEI     I FF      FF      I FF  I EE        I LL  I FF      EEI     //
//      I FF      FFI     I FF          EEI     BBFF      FFI     EEFF    FFI       FFI   I FF      EEI     //
//    I MMMMMMMMMMFF    BBMMMMMMMMFF    LLMMMMMMFFMMFF    I BBMMMMFFFF    I BBMMMMMMLL  I MMMMFF  BBMMBBI   //
//                                                                I FF                                      //
//                                                                FFLL                                      //
//                                                          BBMMMMEE                                        //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                              Dragon System                                               //
//                                                                                                          //
//                               (c) Copyright 2006-2009, Scottyu China                                     //
//                                                                                                          //
//                                           All Rights Reserved                                            //
//                                                                                                          //
// File    : ImgDecode.h                                                                                    //
// By      : scottyu                                                                                        //
// Version : V1.00                                                                                          //
// Time    : 2008-11-03 9:39:47                                                                             //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
// HISTORY                                                                                                  //
//                                                                                                          //
// 1 2008-11-03 9:39:50                                                                                     //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//


#ifndef __IMAGE_DECODE_H____
#define __IMAGE_DECODE_H____	1

//------------------------------------------------------------------------------------------------------------
#define PLUGIN_TYPE				IMGDECODE_PLUGIN_TYPE
#define PLUGIN_NAME				"imgDecode"				//scott note
#define PLUGIN_VERSION			0x0100
#define PLUGIN_AUTHOR			"scottyu"
#define PLUGIN_COPYRIGHT		"scottyu"

//------------------------------------------------------------------------------------------------------------
//插件的通用接口
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// image 解析接口
//------------------------------------------------------------------------------------------------------------
typedef void * 		HIMAGE;

typedef void * 		HIMAGEITEM;

extern   HIMAGE 		Img_Open		(char * ImageFile);

extern   HIMAGEITEM 	Img_OpenItem	(HIMAGE hImage, char * MainType, char * subType);

extern   long long 		Img_GetItemSize	(HIMAGE hImage, HIMAGEITEM hItem);

extern   uint 			Img_GetItemStart(HIMAGE hImage, HIMAGEITEM hItem);

extern   uint 			Img_ReadItem	(HIMAGE hImage, HIMAGEITEM hItem, void *buffer, uint buffer_size);

extern   int 			Img_CloseItem	(HIMAGE hImage, HIMAGEITEM hItem);

extern   void 	 		Img_Close		(HIMAGE hImage);

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //__IMAGE_DECODE_H____

