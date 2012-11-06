/*
*******************************************************************************
*                                            Aone project ---- usb peripheral module
*
* File Name 	: encrypt.c
*
* Author 		: javen
*
* Version 		: 1.0
*
* Date 			: 2008.04.24
*
* Description 	: 加密和解密
*
* History 		:
*
********************************************************************************************************************
*/

#include    "twofish/twofish.h"
#include    "encrypt.h"

#define     CODE_KEY_LEN    32

HTF    handle = NULL;

/*
***************************************************************
*                    get_code_key
*
* Description:
*     获得密匙
* Parameters:
*     void
* Return value:
*     0		:   success
*     !0    :   fail
* History:
*
***************************************************************
*/
int  get_code_key(uint * key_new, uint keylen)
{
    uint  i = 0;

    //暂时用明文的key，等找到好的算法以后再替换
 //   _mem_cpy(key_new, key_old, keylen);

    key_new[0] = 5;
	key_new[1] = 4;
	for(i = 2; i < keylen; i++){
		key_new[i] = key_new[i-1] + key_new[i-2];
	}

    return 0;
}

/*
***************************************************************
*                    init_code
*
* Description:
*     加密算法初始化
* Parameters:
*     void
* Return value:
*     0		:   success
*     !0    :   fail
* History:
*
***************************************************************
*/
int  init_code(void)
{
    int  ret = 0;
	uint  key_new[CODE_KEY_LEN];

	memset((void *)key_new, 0, CODE_KEY_LEN * sizeof(uint));

	ret = get_code_key(key_new, CODE_KEY_LEN);
    if(ret != 0){
		printf("ERR: init_code, get_code_key failed\n");
		return -1;
	}

    handle = TFInitial(key_new, CODE_KEY_LEN);
	if(handle == NULL)
	{
		printf("ERR: init_code, TFInitial failed\n");
		return -1;
	}

    return 0;
}

/*
***************************************************************
*                    encode
*
* Description:
*     加密
* Parameters:
*     ibuf  :  input.  originality buffer
*     obuf  :  output. cryptograph buffer
*     len   :  input.  buffer lenght
* Return value:
*     0		:   success
*     !0    :   fail
* History:
*
***************************************************************
*/
uint  encode(void * ibuf, void * obuf, uint len)
{
    printf("ERR: encode not support\n");

    return 0;
}

/*
***************************************************************
*                    decode
*
* Description:
*     解密
* Parameters:
*     ibuf  :  input.  originality buffer
*     obuf  :  output. cryptograph buffer
*     len   :  input.  buffer lenght
* Return value:
*     0		:   success
*     !0    :   fail
* History:
*
***************************************************************
*/
uint  decode(void * ibuf, void * obuf, uint len)
{
	//return TFDecode(handle, ibuf, len, obuf);
	TFDecode(handle, ibuf, len, obuf);

	return (uint)obuf;
}
//int  decode(uint src_buf, uint dest_buf, uint len, uint *buf_addr)
//{
//	void *ibuf, *obuf;
//
//	ibuf = (void *)src_buf;
//	obuf = (void *)dest_buf;
//
//	TFDecode(handle, ibuf, len, obuf);
//	*buf_addr = dest_buf;
//
//	return 0;
//}

/*
***************************************************************
*                    exit_code
*
* Description:
*     关闭
* Parameters:
*     void
* Return value:
*     0		:   success
*     !0    :   fail
* History:
*
***************************************************************
*/
int  exit_code(void)
{
    return 0;
}
