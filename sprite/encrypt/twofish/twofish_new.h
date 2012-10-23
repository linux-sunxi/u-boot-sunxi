#ifndef __twofish_h__
#define __twofish_h__

#include    "twofish.h"

#define rotl(x,n)   (((x) << ((__u32)(n))) | ((x) >> (32 - (__u32)(n))))
#define rotr(x,n)   (((x) >> ((__u32)(n))) | ((x) << (32 - (__u32)(n))))
#define rotl16(x,n)   (((x) << ((u16)(n))) | ((x) >> (16 - (u16)(n))))
#define rotr16(x,n)   (((x) >> ((u16)(n))) | ((x) << (16 - (u16)(n))))
#define byteswap(x)	((rotl(x, 8) & 0x00ff00ff) | (rotr(x, 8) & 0xff00ff00))

typedef struct
{
	uint  k_len;
	uint  l_key[40];
	uint  s_key[4];

	uint qt_gen;
	unsigned char q_tab[2][256];

	uint mt_gen;
	uint m_tab[4][256];

	uint mk_tab[4][256];
}
__attribute__ ((packed)) TWI;

void twofish_new_set_key(TWI *pkey,const uint in_key[],const uint key_len);
void twofish_new_encrypt(TWI *pkey,uint *in_blk);
//void twofish_new_decrypt(TWI *pkey,uint *in_blk);
void twofish_new_decrypt(TWI * pkey, uint * in_blk, uint * out_blk);

#endif /* !__twofish_h__ */
