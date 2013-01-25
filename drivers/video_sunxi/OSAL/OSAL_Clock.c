/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm boot sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include "OSAL_Clock.h"

struct pll_freq_cfg_tbl{
    __u8    FactorN;
    __u8    FactorK;
    __u8    FactorM;
    __u8    FactorP;
    __u32   Pll;
};

/* normal pll(3/4/7/8/9/10) parameter table, frequency = 24*N/M */
static struct pll_freq_cfg_tbl    PllxTbl[] = {
    {1 , 0, 15,0, 3000000  }, /* freq = (3M * 0  ), index = 0  */
    {1 , 0, 15,0, 3000000  }, /* freq = (3M * 1  ), index = 1  */
    {1 , 0, 7, 0, 6000000  }, /* freq = (3M * 2  ), index = 2  */
    {2 , 0, 7, 0, 9000000  }, /* freq = (3M * 3  ), index = 3  */
    {1 , 0, 3, 0, 12000000 }, /* freq = (3M * 4  ), index = 4  */
    {4 , 0, 7, 0, 15000000 }, /* freq = (3M * 5  ), index = 5  */
    {2 , 0, 3, 0, 18000000 }, /* freq = (3M * 6  ), index = 6  */
    {6 , 0, 7, 0, 21000000 }, /* freq = (3M * 7  ), index = 7  */
    {1 , 0, 1, 0, 24000000 }, /* freq = (3M * 8  ), index = 8  */
    {8 , 0, 7, 0, 27000000 }, /* freq = (3M * 9  ), index = 9  */
    {4 , 0, 3, 0, 30000000 }, /* freq = (3M * 10 ), index = 10 */
    {10, 0, 7, 0, 33000000 }, /* freq = (3M * 11 ), index = 11 */
    {2 , 0, 1, 0, 36000000 }, /* freq = (3M * 12 ), index = 12 */
    {12, 0, 7, 0, 39000000 }, /* freq = (3M * 13 ), index = 13 */
    {6 , 0, 3, 0, 42000000 }, /* freq = (3M * 14 ), index = 14 */
    {14, 0, 7, 0, 45000000 }, /* freq = (3M * 15 ), index = 15 */
    {1 , 0, 0, 0, 48000000 }, /* freq = (3M * 16 ), index = 16 */
    {16, 0, 7, 0, 51000000 }, /* freq = (3M * 17 ), index = 17 */
    {8 , 0, 3, 0, 54000000 }, /* freq = (3M * 18 ), index = 18 */
    {18, 0, 7, 0, 57000000 }, /* freq = (3M * 19 ), index = 19 */
    {4 , 0, 1, 0, 60000000 }, /* freq = (3M * 20 ), index = 20 */
    {20, 0, 7, 0, 63000000 }, /* freq = (3M * 21 ), index = 21 */
    {10, 0, 3, 0, 66000000 }, /* freq = (3M * 22 ), index = 22 */
    {22, 0, 7, 0, 69000000 }, /* freq = (3M * 23 ), index = 23 */
    {2 , 0, 0, 0, 72000000 }, /* freq = (3M * 24 ), index = 24 */
    {24, 0, 7, 0, 75000000 }, /* freq = (3M * 25 ), index = 25 */
    {12, 0, 3, 0, 78000000 }, /* freq = (3M * 26 ), index = 26 */
    {12, 0, 3, 0, 78000000 }, /* freq = (3M * 27 ), index = 27 */
    {6 , 0, 1, 0, 84000000 }, /* freq = (3M * 28 ), index = 28 */
    {14, 0, 3, 0, 90000000 }, /* freq = (3M * 29 ), index = 29 */
    {14, 0, 3, 0, 90000000 }, /* freq = (3M * 30 ), index = 30 */
    {14, 0, 3, 0, 90000000 }, /* freq = (3M * 31 ), index = 31 */
    {3 , 0, 0, 0, 96000000 }, /* freq = (3M * 32 ), index = 32 */
    {3 , 0, 0, 0, 96000000 }, /* freq = (3M * 33 ), index = 33 */
    {16, 0, 3, 0, 102000000}, /* freq = (3M * 34 ), index = 34 */
    {16, 0, 3, 0, 102000000}, /* freq = (3M * 35 ), index = 35 */
    {8 , 0, 1, 0, 108000000}, /* freq = (3M * 36 ), index = 36 */
    {8 , 0, 1, 0, 108000000}, /* freq = (3M * 37 ), index = 37 */
    {18, 0, 3, 0, 114000000}, /* freq = (3M * 38 ), index = 38 */
    {4 , 0, 0, 0, 120000000}, /* freq = (3M * 39 ), index = 39 */
    {4 , 0, 0, 0, 120000000}, /* freq = (3M * 40 ), index = 40 */
    {4 , 0, 0, 0, 120000000}, /* freq = (3M * 41 ), index = 41 */
    {20, 0, 3, 0, 126000000}, /* freq = (3M * 42 ), index = 42 */
    {20, 0, 3, 0, 126000000}, /* freq = (3M * 43 ), index = 43 */
    {10, 0, 1, 0, 132000000}, /* freq = (3M * 44 ), index = 44 */
    {10, 0, 1, 0, 132000000}, /* freq = (3M * 45 ), index = 45 */
    {22, 0, 3, 0, 138000000}, /* freq = (3M * 46 ), index = 46 */
    {22, 0, 3, 0, 138000000}, /* freq = (3M * 47 ), index = 47 */
    {5 , 0, 0, 0, 144000000}, /* freq = (3M * 48 ), index = 48 */
    {5 , 0, 0, 0, 144000000}, /* freq = (3M * 49 ), index = 49 */
    {24, 0, 3, 0, 150000000}, /* freq = (3M * 50 ), index = 50 */
    {24, 0, 3, 0, 150000000}, /* freq = (3M * 51 ), index = 51 */
    {12, 0, 1, 0, 156000000}, /* freq = (3M * 52 ), index = 52 */
    {12, 0, 1, 0, 156000000}, /* freq = (3M * 53 ), index = 53 */
    {12, 0, 1, 0, 156000000}, /* freq = (3M * 54 ), index = 54 */
    {6 , 0, 0, 0, 168000000}, /* freq = (3M * 55 ), index = 55 */
    {6 , 0, 0, 0, 168000000}, /* freq = (3M * 56 ), index = 56 */
    {6 , 0, 0, 0, 168000000}, /* freq = (3M * 57 ), index = 57 */
    {6 , 0, 0, 0, 168000000}, /* freq = (3M * 58 ), index = 58 */
    {14, 0, 1, 0, 180000000}, /* freq = (3M * 59 ), index = 59 */
    {14, 0, 1, 0, 180000000}, /* freq = (3M * 60 ), index = 60 */
    {14, 0, 1, 0, 180000000}, /* freq = (3M * 61 ), index = 61 */
    {14, 0, 1, 0, 180000000}, /* freq = (3M * 62 ), index = 62 */
    {7 , 0, 0, 0, 192000000}, /* freq = (3M * 63 ), index = 63 */
    {7 , 0, 0, 0, 192000000}, /* freq = (3M * 64 ), index = 64 */
    {7 , 0, 0, 0, 192000000}, /* freq = (3M * 65 ), index = 65 */
    {24, 0, 2, 0, 200000000}, /* freq = (3M * 66 ), index = 66 */
    {24, 0, 2, 0, 200000000}, /* freq = (3M * 67 ), index = 67 */
    {16, 0, 1, 0, 204000000}, /* freq = (3M * 68 ), index = 68 */
    {16, 0, 1, 0, 204000000}, /* freq = (3M * 69 ), index = 69 */
    {16, 0, 1, 0, 204000000}, /* freq = (3M * 70 ), index = 70 */
    {8 , 0, 0, 0, 216000000}, /* freq = (3M * 71 ), index = 71 */
    {8 , 0, 0, 0, 216000000}, /* freq = (3M * 72 ), index = 72 */
    {8 , 0, 0, 0, 216000000}, /* freq = (3M * 73 ), index = 73 */
    {8 , 0, 0, 0, 216000000}, /* freq = (3M * 74 ), index = 74 */
    {18, 0, 1, 0, 228000000}, /* freq = (3M * 75 ), index = 75 */
    {18, 0, 1, 0, 228000000}, /* freq = (3M * 76 ), index = 76 */
    {18, 0, 1, 0, 228000000}, /* freq = (3M * 77 ), index = 77 */
    {18, 0, 1, 0, 228000000}, /* freq = (3M * 78 ), index = 78 */
    {9 , 0, 0, 0, 240000000}, /* freq = (3M * 79 ), index = 79 */
    {9 , 0, 0, 0, 240000000}, /* freq = (3M * 80 ), index = 80 */
    {9 , 0, 0, 0, 240000000}, /* freq = (3M * 81 ), index = 81 */
    {9 , 0, 0, 0, 240000000}, /* freq = (3M * 82 ), index = 82 */
    {20, 0, 1, 0, 252000000}, /* freq = (3M * 83 ), index = 83 */
    {20, 0, 1, 0, 252000000}, /* freq = (3M * 84 ), index = 84 */
    {20, 0, 1, 0, 252000000}, /* freq = (3M * 85 ), index = 85 */
    {20, 0, 1, 0, 252000000}, /* freq = (3M * 86 ), index = 86 */
    {10, 0, 0, 0, 264000000}, /* freq = (3M * 87 ), index = 87 */
    {10, 0, 0, 0, 264000000}, /* freq = (3M * 88 ), index = 88 */
    {22, 0, 1, 0, 276000000}, /* freq = (3M * 89 ), index = 89 */
    {22, 0, 1, 0, 276000000}, /* freq = (3M * 90 ), index = 90 */
    {22, 0, 1, 0, 276000000}, /* freq = (3M * 91 ), index = 91 */
    {22, 0, 1, 0, 276000000}, /* freq = (3M * 92 ), index = 92 */
    {11, 0, 0, 0, 288000000}, /* freq = (3M * 93 ), index = 93 */
    {11, 0, 0, 0, 288000000}, /* freq = (3M * 94 ), index = 94 */
    {11, 0, 0, 0, 288000000}, /* freq = (3M * 95 ), index = 95 */
    {11, 0, 0, 0, 288000000}, /* freq = (3M * 96 ), index = 96 */
    {11, 0, 0, 0, 288000000}, /* freq = (3M * 97 ), index = 97 */
    {11, 0, 0, 0, 288000000}, /* freq = (3M * 98 ), index = 98 */
    {24, 0, 1, 0, 300000000}, /* freq = (3M * 99 ), index = 99 */
    {24, 0, 1, 0, 300000000}, /* freq = (3M * 100), index = 100*/
    {24, 0, 1, 0, 300000000}, /* freq = (3M * 101), index = 101*/
    {24, 0, 1, 0, 300000000}, /* freq = (3M * 102), index = 102*/
    {12, 0, 0, 0, 312000000}, /* freq = (3M * 103), index = 103*/
    {12, 0, 0, 0, 312000000}, /* freq = (3M * 104), index = 104*/
    {12, 0, 0, 0, 312000000}, /* freq = (3M * 105), index = 105*/
    {12, 0, 0, 0, 312000000}, /* freq = (3M * 106), index = 106*/
    {12, 0, 0, 0, 312000000}, /* freq = (3M * 107), index = 107*/
    {12, 0, 0, 0, 312000000}, /* freq = (3M * 108), index = 108*/
    {12, 0, 0, 0, 312000000}, /* freq = (3M * 109), index = 109*/
    {13, 0, 0, 0, 336000000}, /* freq = (3M * 110), index = 110*/
    {13, 0, 0, 0, 336000000}, /* freq = (3M * 111), index = 111*/
    {13, 0, 0, 0, 336000000}, /* freq = (3M * 112), index = 112*/
    {13, 0, 0, 0, 336000000}, /* freq = (3M * 113), index = 113*/
    {13, 0, 0, 0, 336000000}, /* freq = (3M * 114), index = 114*/
    {13, 0, 0, 0, 336000000}, /* freq = (3M * 115), index = 115*/
    {13, 0, 0, 0, 336000000}, /* freq = (3M * 116), index = 116*/
    {13, 0, 0, 0, 336000000}, /* freq = (3M * 117), index = 117*/
    {14, 0, 0, 0, 360000000}, /* freq = (3M * 118), index = 118*/
    {14, 0, 0, 0, 360000000}, /* freq = (3M * 119), index = 119*/
    {14, 0, 0, 0, 360000000}, /* freq = (3M * 120), index = 120*/
    {14, 0, 0, 0, 360000000}, /* freq = (3M * 121), index = 121*/
    {14, 0, 0, 0, 360000000}, /* freq = (3M * 122), index = 122*/
    {14, 0, 0, 0, 360000000}, /* freq = (3M * 123), index = 123*/
    {14, 0, 0, 0, 360000000}, /* freq = (3M * 124), index = 124*/
    {14, 0, 0, 0, 360000000}, /* freq = (3M * 125), index = 125*/
    {15, 0, 0, 0, 384000000}, /* freq = (3M * 126), index = 126*/
    {15, 0, 0, 0, 384000000}, /* freq = (3M * 127), index = 127*/
    {15, 0, 0, 0, 384000000}, /* freq = (3M * 128), index = 128*/
    {15, 0, 0, 0, 384000000}, /* freq = (3M * 129), index = 129*/
    {15, 0, 0, 0, 384000000}, /* freq = (3M * 130), index = 130*/
    {15, 0, 0, 0, 384000000}, /* freq = (3M * 131), index = 131*/
    {15, 0, 0, 0, 384000000}, /* freq = (3M * 132), index = 132*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 133), index = 133*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 134), index = 134*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 135), index = 135*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 136), index = 136*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 137), index = 137*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 138), index = 138*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 139), index = 139*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 140), index = 140*/
    {16, 0, 0, 0, 408000000}, /* freq = (3M * 141), index = 141*/
    {17, 0, 0, 0, 432000000}, /* freq = (3M * 142), index = 142*/
    {17, 0, 0, 0, 432000000}, /* freq = (3M * 143), index = 143*/
    {17, 0, 0, 0, 432000000}, /* freq = (3M * 144), index = 144*/
    {17, 0, 0, 0, 432000000}, /* freq = (3M * 145), index = 145*/
    {17, 0, 0, 0, 432000000}, /* freq = (3M * 146), index = 146*/
    {17, 0, 0, 0, 432000000}, /* freq = (3M * 147), index = 147*/
    {17, 0, 0, 0, 432000000}, /* freq = (3M * 148), index = 148*/
    {17, 0, 0, 0, 432000000}, /* freq = (3M * 149), index = 149*/
    {18, 0, 0, 0, 456000000}, /* freq = (3M * 150), index = 150*/
    {18, 0, 0, 0, 456000000}, /* freq = (3M * 151), index = 151*/
    {18, 0, 0, 0, 456000000}, /* freq = (3M * 152), index = 152*/
    {18, 0, 0, 0, 456000000}, /* freq = (3M * 153), index = 153*/
    {18, 0, 0, 0, 456000000}, /* freq = (3M * 154), index = 154*/
    {18, 0, 0, 0, 456000000}, /* freq = (3M * 155), index = 155*/
    {18, 0, 0, 0, 456000000}, /* freq = (3M * 156), index = 156*/
    {19, 0, 0, 0, 480000000}, /* freq = (3M * 157), index = 157*/
    {19, 0, 0, 0, 480000000}, /* freq = (3M * 158), index = 158*/
    {19, 0, 0, 0, 480000000}, /* freq = (3M * 159), index = 159*/
    {19, 0, 0, 0, 480000000}, /* freq = (3M * 160), index = 160*/
    {19, 0, 0, 0, 480000000}, /* freq = (3M * 161), index = 161*/
    {19, 0, 0, 0, 480000000}, /* freq = (3M * 162), index = 162*/
    {19, 0, 0, 0, 480000000}, /* freq = (3M * 163), index = 163*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 164), index = 164*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 165), index = 165*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 166), index = 166*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 167), index = 167*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 168), index = 168*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 169), index = 169*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 170), index = 170*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 171), index = 171*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 172), index = 172*/
    {20, 0, 0, 0, 504000000}, /* freq = (3M * 173), index = 173*/
    {21, 0, 0, 0, 528000000}, /* freq = (3M * 174), index = 174*/
    {21, 0, 0, 0, 528000000}, /* freq = (3M * 175), index = 175*/
    {21, 0, 0, 0, 528000000}, /* freq = (3M * 176), index = 176*/
    {21, 0, 0, 0, 528000000}, /* freq = (3M * 177), index = 177*/
    {21, 0, 0, 0, 528000000}, /* freq = (3M * 178), index = 178*/
    {21, 0, 0, 0, 528000000}, /* freq = (3M * 179), index = 179*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 180), index = 180*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 181), index = 181*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 182), index = 182*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 183), index = 183*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 184), index = 184*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 185), index = 185*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 186), index = 186*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 187), index = 187*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 188), index = 188*/
    {22, 0, 0, 0, 552000000}, /* freq = (3M * 189), index = 189*/
    {23, 0, 0, 0, 576000000}, /* freq = (3M * 190), index = 190*/
    {23, 0, 0, 0, 576000000}, /* freq = (3M * 191), index = 191*/
    {23, 0, 0, 0, 576000000}, /* freq = (3M * 192), index = 192*/
    {23, 0, 0, 0, 576000000}, /* freq = (3M * 193), index = 193*/
    {23, 0, 0, 0, 576000000}, /* freq = (3M * 194), index = 194*/
    {23, 0, 0, 0, 576000000}, /* freq = (3M * 195), index = 195*/
    {23, 0, 0, 0, 576000000}, /* freq = (3M * 196), index = 196*/
    {24, 0, 0, 0, 600000000}, /* freq = (3M * 197), index = 197*/
    {24, 0, 0, 0, 600000000}, /* freq = (3M * 198), index = 198*/
    {24, 0, 0, 0, 600000000}, /* freq = (3M * 199), index = 199*/
    {24, 0, 0, 0, 600000000}, /* freq = (3M * 200), index = 200*/
};

#if 1
__s32 OSAL_CCMU_SetSrcFreq(__u32 nSclkNo, __u32 nFreq)
{
    //__inf("OSAL_CCMU_SetSrcFreq,%d,%d\n", nSclkNo, nFreq);
    __u32 reg_val;
    nFreq /= 3000000;

    if(nFreq == 234)  //702M
    {
        reg_val = ((4-1)<<0) | ((117-1)<<8);
    }
    else
    {
        reg_val = (PllxTbl[nFreq-1].FactorM<<0) | (PllxTbl[nFreq-1].FactorN<<8);
    }

    if(nSclkNo == SYS_CLK_PLL3)
    {
        sys_put_wvalue(0x01c20010,0x81000000 | reg_val);
    }
    else if(nSclkNo == SYS_CLK_PLL7)
    {
        sys_put_wvalue(0x01c20030,0x81000000 | reg_val);
    }
    else if(nSclkNo == SYS_CLK_PLL9)
    {
        sys_put_wvalue(0x01c20044,0x81000000 | reg_val);
    }
    else if(nSclkNo == SYS_CLK_PLL10)
    {
        sys_put_wvalue(0x01c20048,0x81000000 | reg_val);
    }
    return 0;
}


__u32 OSAL_CCMU_GetSrcFreq(__u32 nSclkNo)
{
    __u32 value = 0;
    __u32 div = 0;
    __u32 factor = 0;

    //__inf("OSAL_CCMU_GetSrcFreq,%d\n",nSclkNo);

    if(nSclkNo == SYS_CLK_PLL3)
    {
        factor = (sys_get_wvalue(0x01c20010) >> 8)&0x7f;
        div = sys_get_wvalue(0x01c20010) & 0xf;
    }
    else if(nSclkNo == SYS_CLK_PLL7)
    {
        factor = (sys_get_wvalue(0x01c20030) >> 8)&0x7f;
        div = sys_get_wvalue(0x01c20030) & 0xf;
    }
    else if(nSclkNo == SYS_CLK_PLL9)
    {
        factor = (sys_get_wvalue(0x01c20044) >> 8)&0x7f;
        div = sys_get_wvalue(0x01c20044) & 0xf;
    }
    else if(nSclkNo == SYS_CLK_PLL10)
    {
        factor = (sys_get_wvalue(0x01c20048) >> 8)&0x7f;
        div = sys_get_wvalue(0x01c20048) & 0xf;
    }

    value = ((24 * (factor+1)) / (div+1)) * 1000000;

    return value;
}

__hdle OSAL_CCMU_OpenMclk(__s32 nMclkNo)
{
    return (__hdle)nMclkNo;
}

__s32  OSAL_CCMU_CloseMclk(__hdle hMclk)
{
    return 0;
}

__s32 OSAL_CCMU_SetMclkSrc(__hdle hMclk, __u32 nSclkNo)
{
    __u32 clk_src = 0;
    __u32 value = 0;

    //__inf("OSAL_CCMU_SetMclkSrc,%d,%d\n", hMclk, nSclkNo);

    if(hMclk == MOD_CLK_LCD0CH0 || hMclk == MOD_CLK_LCD1CH0)
    {
        if(nSclkNo == SYS_CLK_PLL3)
        {
            clk_src = 0;
        }
        else if(nSclkNo == SYS_CLK_PLL7)
        {
            clk_src = 1;
        }
        else if(nSclkNo == SYS_CLK_PLL3X2)
        {
            clk_src = 2;
        }
        else if(nSclkNo == SYS_CLK_PLL7X2)
        {
            clk_src = 3;
        }else if(nSclkNo == SYS_CLK_MIPIPLL)
        {
            clk_src = 4;
        }
    }
    else if(hMclk == MOD_CLK_LCD0CH1 || hMclk == MOD_CLK_LCD0CH1 || hMclk ==  MOD_CLK_HDMI
        || hMclk == MOD_CLK_MIPIDSIP || hMclk == MOD_CLK_MIPIDSIS)
    {
        if(nSclkNo == SYS_CLK_PLL3)
        {
            clk_src = 0;
        }
        else if(nSclkNo == SYS_CLK_PLL7)
        {
            clk_src = 1;
        }
        else if(nSclkNo == SYS_CLK_PLL3X2)
        {
            clk_src = 2;
        }
        else if(nSclkNo == SYS_CLK_PLL7X2)
        {
            clk_src = 3;
        }
    }
    else if(hMclk == MOD_CLK_DEBE0 || hMclk == MOD_CLK_DEBE1 || hMclk == MOD_CLK_DEFE0
        || hMclk == MOD_CLK_DEFE1 || hMclk == MOD_CLK_IEPDEU0 || hMclk ==  MOD_CLK_IEPDEU1
        || hMclk == MOD_CLK_IEPDRC0 || hMclk == MOD_CLK_IEPDRC1 )
    {
        if(nSclkNo == SYS_CLK_PLL3)
        {
            clk_src = 0;
        }
        else if(nSclkNo == SYS_CLK_PLL7)
        {
            clk_src = 1;
        }
        else if(nSclkNo == SYS_CLK_PLL6x2)
        {
            clk_src = 2;
        }
        else if(nSclkNo == SYS_CLK_PLL8)
        {
            clk_src = 3;
        }
        else if(nSclkNo == SYS_CLK_PLL9)
        {
            clk_src = 4;
        }
        else if(nSclkNo == SYS_CLK_PLL10)
        {
            clk_src = 5;
        }
    }


    if(hMclk == MOD_CLK_LCD0CH0)
    {
        value = sys_get_wvalue(0x01c20118) & (~(0x7<<24));
        sys_put_wvalue(0x01c20118, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_LCD1CH0)
    {
        value = sys_get_wvalue(0x01c2011c) & (~(0x7<<24));
        sys_put_wvalue(0x01c2011c, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_LCD0CH1)
    {
        value = sys_get_wvalue(0x01c2012c) & (~(0x3<<24));
        sys_put_wvalue(0x01c2012c, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_LCD1CH1)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0x3<<24));
        sys_put_wvalue(0x01c20130, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_HDMI)
    {
        value = sys_get_wvalue(0x01c20150) & (~(0x3<<24));
        sys_put_wvalue(0x01c20150, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20104) & (~(0x7<<24));
        sys_put_wvalue(0x01c20104, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20108) & (~(0x7<<24));
        sys_put_wvalue(0x01c20108, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c2010c) & (~(0x7<<24));
        sys_put_wvalue(0x01c2010c, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20110) & (~(0x7<<24));
        sys_put_wvalue(0x01c20110, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_MIPIDSIS)
    {
        value = sys_get_wvalue(0x01c20168) & (~(0x3<<24));
        sys_put_wvalue(0x01c20168, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_MIPIDSIP)
    {
        value = sys_get_wvalue(0x01c20168) & (~(0x3<<8));
        sys_put_wvalue(0x01c20168, value | (clk_src<<8));
    }
    else if(hMclk == MOD_CLK_IEPDRC0)
    {
        value = sys_get_wvalue(0x01c20180) & (~(0x7<<24));
        sys_put_wvalue(0x01c20180, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_IEPDRC1)
    {
        value = sys_get_wvalue(0x01c20184) & (~(0x7<<24));
        sys_put_wvalue(0x01c20184, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_IEPDEU0)
    {
        value = sys_get_wvalue(0x01c20188) & (~(0x7<<24));
        sys_put_wvalue(0x01c20188, value | (clk_src<<24));
    }
    else if(hMclk == MOD_CLK_IEPDEU1)
    {
        value = sys_get_wvalue(0x01c2018c) & (~(0x7<<24));
        sys_put_wvalue(0x01c2018c, value | (clk_src<<24));
    }
    return 0;
}

__s32 OSAL_CCMU_GetMclkSrc(__hdle hMclk)
{
    return 0;
}

__s32 OSAL_CCMU_SetMclkDiv(__hdle hMclk, __s32 nDiv)
{
    __u32 value = 0;

    //__inf("OSAL_CCMU_SetMclkDiv,%d,%d\n", hMclk, nDiv);

    nDiv = (nDiv == 0)? 1:nDiv;

    if(hMclk == MOD_CLK_LCD0CH0)
    {
        value = sys_get_wvalue(0x01c20118) & (~(0xf<<0));
        sys_put_wvalue(0x01c20118, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_LCD1CH0)
    {
        value = sys_get_wvalue(0x01c2011c) & (~(0xf<<0));
        sys_put_wvalue(0x01c2011c, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_LCD0CH1)
    {
        value = sys_get_wvalue(0x01c2012c) & (~(0xf<<0));
        sys_put_wvalue(0x01c2012c, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_LCD1CH1)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0xf<<0));
        sys_put_wvalue(0x01c20130, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_HDMI)
    {
        value = sys_get_wvalue(0x01c20150) & (~(0xf<<0));
        sys_put_wvalue(0x01c20150, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20104) & (~(0xf<<0));
        sys_put_wvalue(0x01c20104, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20108) & (~(0xf<<0));
        sys_put_wvalue(0x01c20108, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c2010c) & (~(0xf<<0));
        sys_put_wvalue(0x01c2010c, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20110) & (~(0xf<<0));
        sys_put_wvalue(0x01c20110, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_MIPIDSIS)
    {
        value = sys_get_wvalue(0x01c20168) & (~(0xf<<16));
        sys_put_wvalue(0x01c20168, value | ((nDiv-1)<<16));
    }
    else if(hMclk == MOD_CLK_MIPIDSIP)
    {
        value = sys_get_wvalue(0x01c20168) & (~(0xf<<0));
        sys_put_wvalue(0x01c20168, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_IEPDRC0)
    {
        value = sys_get_wvalue(0x01c20180) & (~(0xf<<0));
        sys_put_wvalue(0x01c20180, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_IEPDRC1)
    {
        value = sys_get_wvalue(0x01c20184) & (~(0xf<<0));
        sys_put_wvalue(0x01c20184, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_IEPDEU0)
    {
        value = sys_get_wvalue(0x01c20188) & (~(0xf<<0));
        sys_put_wvalue(0x01c20188, value | ((nDiv-1)<<0));
    }
    else if(hMclk == MOD_CLK_IEPDEU1)
    {
        value = sys_get_wvalue(0x01c2018c) & (~(0xf<<0));
        sys_put_wvalue(0x01c2018c, value | ((nDiv-1)<<0));
    }

    return 0;
}

__u32 OSAL_CCMU_GetMclkDiv(__hdle hMclk)
{
    return 0;
}

__s32 OSAL_CCMU_MclkOnOff(__hdle hMclk, __s32 bOnOff)
{
    __u32 value = 0;

    //__inf("OSAL_CCMU_MclkOnOff,%d,%d\n", hMclk, bOnOff);

    if(hMclk == MOD_CLK_LCD0CH0)
    {
        value = sys_get_wvalue(0x01c20118) & (~(0x1<<31));
        sys_put_wvalue(0x01c20118, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_LCD1CH0)
    {
        value = sys_get_wvalue(0x01c2011c) & (~(0x1<<31));
        sys_put_wvalue(0x01c2011c, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_LCD0CH1)
    {
        value = sys_get_wvalue(0x01c2012c) & (~(0x1<<31));
        sys_put_wvalue(0x01c2012c, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_LCD1CH1)
    {
        value = sys_get_wvalue(0x01c20130) & (~(0x1<<31));
        sys_put_wvalue(0x01c20130, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_HDMI)
    {
        value = sys_get_wvalue(0x01c20150) & (~(0x1<<31));
        sys_put_wvalue(0x01c20150, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20104) & (~(0x1<<31));
        sys_put_wvalue(0x01c20104, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20108) & (~(0x1<<31));
        sys_put_wvalue(0x01c20108, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c2010c) & (~(0x1<<31));
        sys_put_wvalue(0x01c2010c, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20110) & (~(0x1<<31));
        sys_put_wvalue(0x01c20110, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_MIPIDSIS)
    {
        value = sys_get_wvalue(0x01c20168) & (~(0x1<<31));
        sys_put_wvalue(0x01c20168, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_MIPIDSIP)
    {
        value = sys_get_wvalue(0x01c20168) & (~(0x1<<15));
        sys_put_wvalue(0x01c20168, value | (bOnOff<<15));
    }
    else if(hMclk == MOD_CLK_IEPDRC0)
    {
        value = sys_get_wvalue(0x01c20180) & (~(0x1<<31));
        sys_put_wvalue(0x01c20180, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_IEPDRC1)
    {
        value = sys_get_wvalue(0x01c20184) & (~(0x1<<31));
        sys_put_wvalue(0x01c20184, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_IEPDEU0)
    {
        value = sys_get_wvalue(0x01c20188) & (~(0x1<<31));
        sys_put_wvalue(0x01c20188, value | (bOnOff<<31));
    }
    else if(hMclk == MOD_CLK_IEPDEU1)
    {
        value = sys_get_wvalue(0x01c2018c) & (~(0x1<<31));
        sys_put_wvalue(0x01c2018c, value | (bOnOff<<31));
    }
    else if(hMclk == DRAM_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<26));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<26));
    }
    else if(hMclk == DRAM_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<27));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<27));
    }
    else if(hMclk == DRAM_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<24));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<24));
    }
    else if(hMclk == DRAM_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<25));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<25));
    }
    else if(hMclk == DRAM_CLK_DEU0)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<18));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<18));
    }
    else if(hMclk == DRAM_CLK_DEU1)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<19));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<19));
    }
    else if(hMclk == DRAM_CLK_DRC0)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<16));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<16));
    }
    else if(hMclk == DRAM_CLK_DRC1)
    {
        value = sys_get_wvalue(0x01c20100) & (~(0x1<<17));
        sys_put_wvalue(0x01c20100, value | (bOnOff<<17));
    }
    else if(hMclk == AHB_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<14));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<14));
    }
    else if(hMclk == AHB_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<15));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<15));
    }
    else if(hMclk == AHB_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<12));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<12));
    }
    else if(hMclk == AHB_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<13));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<13));
    }
    else if(hMclk == AHB_CLK_MIPIDSI)
    {
             value = sys_get_wvalue(0x01c20060) & (~(0x1<<1));
             sys_put_wvalue(0x01c20060, value | (bOnOff<<1));
     }
    else if(hMclk == AHB_CLK_HDMI)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<11));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<11));
    }
    else if(hMclk == AHB_CLK_LCD0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<4));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<4));
    }
    else if(hMclk == AHB_CLK_LCD1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<5));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<5));
    }
    else if(hMclk == AHB_CLK_DRC0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<25));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<25));
    }
    else if(hMclk == AHB_CLK_DRC1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<26));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<26));
    }
    else if(hMclk == AHB_CLK_DEU0)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<23));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<23));
    }
    else if(hMclk == AHB_CLK_DEU1)
    {
        value = sys_get_wvalue(0x01c20064) & (~(0x1<<24));
        sys_put_wvalue(0x01c20064, value | (bOnOff<<24));
    }

    return 0;
}

__s32 OSAL_CCMU_MclkReset(__hdle hMclk, __s32 bReset)
{
    __u32 value = 0;

    //__inf("OSAL_CCMU_MclkReset,%d,%d\n", hMclk, bReset);

    bReset = 1-bReset;

    if(hMclk == MOD_CLK_DEFE0)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<14));
        sys_put_wvalue(0x01c202c4, value | (bReset<<14));
    }
    else if(hMclk == MOD_CLK_DEFE1)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<15));
        sys_put_wvalue(0x01c202c4, value | (bReset<<15));
    }
    else if(hMclk == MOD_CLK_DEBE0)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<12));
        sys_put_wvalue(0x01c202c4, value | (bReset<<12));
    }
    else if(hMclk == MOD_CLK_DEBE1)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<13));
        sys_put_wvalue(0x01c202c4, value | (bReset<<13));
    }
    else if(hMclk == MOD_CLK_HDMI)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<11));
        sys_put_wvalue(0x01c202c4, value | (bReset<<11));
    }
    else if(hMclk == MOD_CLK_LCD0CH0)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<4));
        sys_put_wvalue(0x01c202c4, value | (bReset<<4));
    }
    else if(hMclk == MOD_CLK_LCD1CH0)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<5));
        sys_put_wvalue(0x01c202c4, value | (bReset<<5));
    }
    else if(hMclk == MOD_CLK_LVDS)
    {
        value = sys_get_wvalue(0x01c202c8) & (~(0x1<<0));
        sys_put_wvalue(0x01c202c8, value | (bReset<<0));
    }
    else if(hMclk == MOD_CLK_IEPDRC0)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<25));
        sys_put_wvalue(0x01c202c4, value | (bReset<<25));
    }
    else if(hMclk == MOD_CLK_IEPDRC1)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<26));
        sys_put_wvalue(0x01c202c4, value | (bReset<<26));
    }
    else if(hMclk == MOD_CLK_IEPDEU0)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<23));
        sys_put_wvalue(0x01c202c4, value | (bReset<<23));
    }
    else if(hMclk == MOD_CLK_IEPDEU1)
    {
        value = sys_get_wvalue(0x01c202c4) & (~(0x1<<24));
        sys_put_wvalue(0x01c202c4, value | (bReset<<24));
    }
    else if(hMclk == MOD_CLK_MIPIDSIS)
    {
        value = sys_get_wvalue(0x01c202c0) & (~(0x1<<1));
        sys_put_wvalue(0x01c202c0, value | (bReset<<1));
    }
    return 0;
}

#else

typedef __u32 CSP_CCM_sysClkNo_t;


__s32 OSAL_CCMU_SetSrcFreq( CSP_CCM_sysClkNo_t nSclkNo, __u32 nFreq )
{
    return 0;
}

__u32 OSAL_CCMU_GetSrcFreq( CSP_CCM_sysClkNo_t nSclkNo )
{
    return 0;
}

__hdle OSAL_CCMU_OpenMclk( __s32 nMclkNo )
{
    return 0;
}

__s32 OSAL_CCMU_CloseMclk( __hdle hMclk )
{
    return 0;
}

__s32 OSAL_CCMU_SetMclkSrc( __hdle hMclk, CSP_CCM_sysClkNo_t nSclkNo )
{
    return 0;
}

__s32 OSAL_CCMU_GetMclkSrc( __hdle hMclk )
{
    return 0;
}

__s32 OSAL_CCMU_SetMclkDiv( __hdle hMclk, __s32 nDiv )
{
    return 0;
}

__u32 OSAL_CCMU_GetMclkDiv( __hdle hMclk )
{
    return 0;
}

__s32 OSAL_CCMU_MclkOnOff( __hdle hMclk, __s32 bOnOff )
{
    return 0;
}

__s32 OSAL_CCMU_MclkReset(__hdle hMclk, __s32 bReset)
{
    return 0;
}
#endif

