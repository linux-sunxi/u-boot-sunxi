/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON driver sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : sw_clock.c
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
#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/cpu.h>

struct core_pll_freq_tbl {
    int FactorN;
    int FactorK;
    int FactorM;
    int pading;
};

static struct core_pll_freq_tbl pll1_table[] =
{  //N      K   M   pading
	{4,		1,	1,	0},    //Freq1 = 0   , Freq2 = 120 , Index = 0
	{4,		1,	1,	0},    //Freq1 = 6   , Freq2 = 120 , Index = 1
	{4,		1,	1,	0},    //Freq1 = 12  , Freq2 = 120 , Index = 2
	{4,		1,	1,	0},    //Freq1 = 18  , Freq2 = 120 , Index = 3
	{4,		1,	1,	0},    //Freq1 = 24  , Freq2 = 120 , Index = 4
	{4,		1,	1,	0},    //Freq1 = 30  , Freq2 = 120 , Index = 5
	{4,		1,	1,	0},    //Freq1 = 36  , Freq2 = 120 , Index = 6
	{4,		1,	1,	0},    //Freq1 = 42  , Freq2 = 120 , Index = 7
	{4,		1,	1,	0},    //Freq1 = 48  , Freq2 = 120 , Index = 8
	{4,		1,	1,	0},    //Freq1 = 54  , Freq2 = 120 , Index = 9
	{4,		1,	1,	0},    //Freq1 = 60  , Freq2 = 120 , Index = 10
	{4,		1,	1,	0},    //Freq1 = 66  , Freq2 = 120 , Index = 11
	{4,		1,	1,	0},    //Freq1 = 72  , Freq2 = 120 , Index = 12
	{4,		1,	1,	0},    //Freq1 = 78  , Freq2 = 120 , Index = 13
	{4,		1,	1,	0},    //Freq1 = 84  , Freq2 = 120 , Index = 14
	{4,		1,	1,	0},    //Freq1 = 90  , Freq2 = 120 , Index = 15
	{4,		1,	1,	0},    //Freq1 = 96  , Freq2 = 120 , Index = 16
	{4,		1,	1,	0},    //Freq1 = 102 , Freq2 = 120 , Index = 17
	{4,		1,	1,	0},    //Freq1 = 108 , Freq2 = 120 , Index = 18
	{4,		1,	1,	0},    //Freq1 = 114 , Freq2 = 120 , Index = 19
	{4,		1,	1,	0},    //Freq1 = 120 , Freq2 = 120 , Index = 20
	{4,	    1,	1,	0},    //Freq1 = 126 , Freq2 = 120 , Index = 21
	{4,		1,	1,	0},    //Freq1 = 132 , Freq2 = 120 , Index = 22
	{4,		1,	1,	0},    //Freq1 = 138 , Freq2 = 120 , Index = 23
	{5,	    1,	1,	0},    //Freq1 = 144 , Freq2 = 144 , Index = 24
	{5,		1,	1,	0},    //Freq1 = 150 , Freq2 = 144 , Index = 25
	{5,		1,	1,	0},    //Freq1 = 156 , Freq2 = 144 , Index = 26
	{5,		1,	1,	0},    //Freq1 = 162 , Freq2 = 144 , Index = 27
	{6,	    1,	1,	0},    //Freq1 = 168 , Freq2 = 168 , Index = 28
	{6,		1,	1,	0},    //Freq1 = 174 , Freq2 = 168 , Index = 29
	{6,		1,	1,	0},    //Freq1 = 180 , Freq2 = 168 , Index = 30
	{6,		1,	1,	0},    //Freq1 = 186 , Freq2 = 168 , Index = 31
	{7,		1,	1,	0},    //Freq1 = 192 , Freq2 = 192 , Index = 32
	{7,	    1,	1,	0},    //Freq1 = 198 , Freq2 = 192 , Index = 33
	{7,	    1,	1,	0},    //Freq1 = 204 , Freq2 = 192 , Index = 34
	{7,	    1,	1,	0},    //Freq1 = 210 , Freq2 = 192 , Index = 35
	{8,		1,	1,	0},    //Freq1 = 216 , Freq2 = 216 , Index = 36
	{8,		1,	1,	0},    //Freq1 = 222 , Freq2 = 216 , Index = 37
	{8,		1,	1,	0},    //Freq1 = 228 , Freq2 = 216 , Index = 38
	{8,		1,	1,	0},    //Freq1 = 234 , Freq2 = 216 , Index = 39
	{9,		1,	1,	0},    //Freq1 = 240 , Freq2 = 240 , Index = 40
	{9,		1,	1,	0},    //Freq1 = 246 , Freq2 = 240 , Index = 41
	{9, 	1,	1,	0},    //Freq1 = 252 , Freq2 = 240 , Index = 42
	{9, 	1,	1,	0},    //Freq1 = 258 , Freq2 = 240 , Index = 43
	{10,	1,	1,	0},    //Freq1 = 264 , Freq2 = 264 , Index = 44
	{10,	1,	1,	0},    //Freq1 = 270 , Freq2 = 264 , Index = 45
	{10,	1,	1,	0},    //Freq1 = 276 , Freq2 = 264 , Index = 46
	{10,	1,	1,	0},    //Freq1 = 282 , Freq2 = 264 , Index = 47
	{11,	1,	1,	0},    //Freq1 = 288 , Freq2 = 288 , Index = 48
	{11,	1,	1,	0},    //Freq1 = 294 , Freq2 = 288 , Index = 49
	{11,	1,	1,	0},    //Freq1 = 300 , Freq2 = 288 , Index = 50
	{11,	1,	1,	0},    //Freq1 = 306 , Freq2 = 288 , Index = 51
	{12,	1,	1,	0},    //Freq1 = 312 , Freq2 = 312 , Index = 52
	{12,	1,	1,	0},    //Freq1 = 318 , Freq2 = 312 , Index = 53
	{12,	1,	1,	0},    //Freq1 = 324 , Freq2 = 312 , Index = 54
	{12,	1,	1,	0},    //Freq1 = 330 , Freq2 = 312 , Index = 55
	{13,	1,	1,	0},    //Freq1 = 336 , Freq2 = 336 , Index = 56
	{13,	1,	1,	0},    //Freq1 = 342 , Freq2 = 336 , Index = 57
	{13,	1,	1,	0},    //Freq1 = 348 , Freq2 = 336 , Index = 58
	{13,	1,	1,	0},    //Freq1 = 354 , Freq2 = 336 , Index = 59
	{14,	1,	1,	0},    //Freq1 = 360 , Freq2 = 360 , Index = 60
	{14,	1,	1,	0},    //Freq1 = 366 , Freq2 = 360 , Index = 61
	{14,	1,	1,	0},    //Freq1 = 372 , Freq2 = 360 , Index = 62
	{14,	1,	1,	0},    //Freq1 = 378 , Freq2 = 360 , Index = 63
	{15,    1,	1,	0},    //Freq1 = 384 , Freq2 = 384 , Index = 64
	{15,	1,	1,	0},    //Freq1 = 390 , Freq2 = 384 , Index = 65
	{15,	1,	1,	0},    //Freq1 = 396 , Freq2 = 384 , Index = 66
	{15,	1,	1,	0},    //Freq1 = 402 , Freq2 = 384 , Index = 67
	{16,	1,	1,	0},    //Freq1 = 408 , Freq2 = 408 , Index = 68
	{16,	1,	1,	0},    //Freq1 = 414 , Freq2 = 408 , Index = 69
	{16,	1,	1,	0},    //Freq1 = 420 , Freq2 = 408 , Index = 70
	{16,	1,	1,	0},    //Freq1 = 426 , Freq2 = 408 , Index = 71
	{17,	1,	1,	0},    //Freq1 = 432 , Freq2 = 432 , Index = 72
	{17,	1,	1,	0},    //Freq1 = 438 , Freq2 = 432 , Index = 73
	{17,	1,	1,	0},    //Freq1 = 444 , Freq2 = 432 , Index = 74
	{17,	1,	1,	0},    //Freq1 = 450 , Freq2 = 432 , Index = 75
	{18,	1,	1,	0},    //Freq1 = 456 , Freq2 = 456 , Index = 76
	{18,	1,	1,	0},    //Freq1 = 462 , Freq2 = 456 , Index = 77
	{18,	1,	1,	0},    //Freq1 = 468 , Freq2 = 456 , Index = 78
	{18,	1,	1,	0},    //Freq1 = 474 , Freq2 = 456 , Index = 79
	{19,	1,	1,	0},    //Freq1 = 480 , Freq2 = 480 , Index = 80
	{19,	1,	1,	0},    //Freq1 = 486 , Freq2 = 480 , Index = 81
	{19,	1,	1,	0},    //Freq1 = 492 , Freq2 = 480 , Index = 82
	{19,	1,	1,	0},    //Freq1 = 498 , Freq2 = 480 , Index = 83
	{20,	1,	1,	0},    //Freq1 = 504 , Freq2 = 504 , Index = 84
	{20,	1,	1,	0},    //Freq1 = 510 , Freq2 = 504 , Index = 85
	{20,	1,	1,	0},    //Freq1 = 516 , Freq2 = 504 , Index = 86
	{20,	1,	1,	0},    //Freq1 = 522 , Freq2 = 504 , Index = 87
	{21,	1,	1,	0},    //Freq1 = 528 , Freq2 = 528 , Index = 88
	{21,	1,	1,	0},    //Freq1 = 534 , Freq2 = 528 , Index = 89
	{21,	1,	1,	0},    //Freq1 = 540 , Freq2 = 528 , Index = 90
	{21,	1,	1,	0},    //Freq1 = 546 , Freq2 = 528 , Index = 91
	{22,	1,	1,	0},    //Freq1 = 552 , Freq2 = 552 , Index = 92
	{22,	1,	1,	0},    //Freq1 = 558 , Freq2 = 552 , Index = 93
	{22,	1,	1,	0},    //Freq1 = 564 , Freq2 = 552 , Index = 94
	{22,	1,	1,	0},    //Freq1 = 570 , Freq2 = 552 , Index = 95
	{23,	1,	1,	0},    //Freq1 = 576 , Freq2 = 576 , Index = 96
	{23,	1,	1,	0},    //Freq1 = 582 , Freq2 = 576 , Index = 97
	{23,	1,	1,	0},    //Freq1 = 588 , Freq2 = 576 , Index = 98
	{23,	1,	1,	0},    //Freq1 = 594 , Freq2 = 576 , Index = 99
	{24,	1,	1,	0},    //Freq1 = 600 , Freq2 = 600 , Index = 100
	{24,	1,	1,	0},    //Freq1 = 606 , Freq2 = 600 , Index = 101
	{24,	1,	1,	0},    //Freq1 = 612 , Freq2 = 600 , Index = 102
	{24,	1,	1,	0},    //Freq1 = 618 , Freq2 = 600 , Index = 103
	{25,	1,	1,	0},    //Freq1 = 624 , Freq2 = 624 , Index = 104
	{25,	1,	1,	0},    //Freq1 = 630 , Freq2 = 624 , Index = 105
	{25,	1,	1,	0},    //Freq1 = 636 , Freq2 = 624 , Index = 106
	{25,	1,	1,	0},    //Freq1 = 642 , Freq2 = 624 , Index = 107
	{26,	1,	1,	0},    //Freq1 = 648 , Freq2 = 648 , Index = 108
	{26,	1,	1,	0},    //Freq1 = 654 , Freq2 = 648 , Index = 109
	{26,	1,	1,	0},    //Freq1 = 660 , Freq2 = 648 , Index = 110
	{26,	1,	1,	0},    //Freq1 = 666 , Freq2 = 648 , Index = 111
	{27,	1,	1,	0},    //Freq1 = 672 , Freq2 = 672 , Index = 112
	{27,	1,	1,	0},    //Freq1 = 678 , Freq2 = 672 , Index = 113
	{27,	1,	1,	0},    //Freq1 = 684 , Freq2 = 672 , Index = 114
	{27,	1,	1,	0},    //Freq1 = 690 , Freq2 = 672 , Index = 115
	{28,	1,	1,	0},    //Freq1 = 696 , Freq2 = 696 , Index = 116
	{28,	1,	1,	0},    //Freq1 = 702 , Freq2 = 696 , Index = 117
	{28,	1,	1,	0},    //Freq1 = 708 , Freq2 = 696 , Index = 118
	{28,	1,	1,	0},    //Freq1 = 714 , Freq2 = 696 , Index = 119
	{29,	1,	1,	0},    //Freq1 = 720 , Freq2 = 720 , Index = 120
	{29,	1,	1,	0},    //Freq1 = 726 , Freq2 = 720 , Index = 121
	{29,	1,	1,	0},    //Freq1 = 732 , Freq2 = 720 , Index = 122
	{29,	1,	1,	0},    //Freq1 = 738 , Freq2 = 720 , Index = 123
	{30,	1,	1,	0},    //Freq1 = 744 , Freq2 = 744 , Index = 124
	{30,	1,	1,	0},    //Freq1 = 750 , Freq2 = 744 , Index = 125
	{30,	1,	1,	0},    //Freq1 = 756 , Freq2 = 744 , Index = 126
	{30,	1,	1,	0},    //Freq1 = 762 , Freq2 = 744 , Index = 127
	{31,	1,	1,	0},    //Freq1 = 768 , Freq2 = 768 , Index = 128
	{31,	1,	1,	0},    //Freq1 = 774 , Freq2 = 768 , Index = 129
	{31,	1,	1,	0},    //Freq1 = 780 , Freq2 = 768 , Index = 130
	{31,	1,	1,	0},    //Freq1 = 786 , Freq2 = 768 , Index = 131
	{21,	2,	1,	0},    //Freq1 = 792 , Freq2 = 792 , Index = 132
	{21,	2,	1,	0},    //Freq1 = 798 , Freq2 = 792 , Index = 133
	{21,	2,	1,	0},    //Freq1 = 804 , Freq2 = 792 , Index = 134
	{21,	2,	1,	0},    //Freq1 = 810 , Freq2 = 792 , Index = 135
	{21,	2,	1,	0},    //Freq1 = 816 , Freq2 = 792 , Index = 136
	{21,	2,	1,	0},    //Freq1 = 822 , Freq2 = 792 , Index = 137
	{22,	2,	1,	0},    //Freq1 = 828 , Freq2 = 828 , Index = 138
	{22,	2,	1,	0},    //Freq1 = 834 , Freq2 = 828 , Index = 139
	{22,	2,	1,	0},    //Freq1 = 840 , Freq2 = 828 , Index = 140
	{22,	2,	1,	0},    //Freq1 = 846 , Freq2 = 828 , Index = 141
	{22,	2,	1,	0},    //Freq1 = 852 , Freq2 = 828 , Index = 142
	{22,	2,	1,	0},    //Freq1 = 858 , Freq2 = 828 , Index = 143
	{23,	2,	1,	0},    //Freq1 = 864 , Freq2 = 864 , Index = 144
	{23,	2,	1,	0},    //Freq1 = 870 , Freq2 = 864 , Index = 145
	{23,	2,	1,	0},    //Freq1 = 876 , Freq2 = 864 , Index = 146
	{23,	2,	1,	0},    //Freq1 = 882 , Freq2 = 864 , Index = 147
	{23,	2,	1,	0},    //Freq1 = 888 , Freq2 = 864 , Index = 148
	{23,	2,	1,	0},    //Freq1 = 894 , Freq2 = 864 , Index = 149
	{24,	2,	1,	0},    //Freq1 = 900 , Freq2 = 900 , Index = 150
	{24,	2,	1,	0},    //Freq1 = 906 , Freq2 = 900 , Index = 151
	{24,	2,	1,	0},    //Freq1 = 912 , Freq2 = 900 , Index = 152
	{24,	2,	1,	0},    //Freq1 = 918 , Freq2 = 900 , Index = 153
	{24,	2,	1,	0},    //Freq1 = 924 , Freq2 = 900 , Index = 154
	{24,	2,	1,	0},    //Freq1 = 930 , Freq2 = 900 , Index = 155
	{25,	2,	1,	0},    //Freq1 = 936 , Freq2 = 936 , Index = 156
	{25,	2,	1,	0},    //Freq1 = 942 , Freq2 = 936 , Index = 157
	{25,	2,	1,	0},    //Freq1 = 948 , Freq2 = 936 , Index = 158
	{25,	2,	1,	0},    //Freq1 = 954 , Freq2 = 936 , Index = 159
	{25,	2,	1,	0},    //Freq1 = 960 , Freq2 = 936 , Index = 160
	{25,	2,	1,	0},    //Freq1 = 966 , Freq2 = 936 , Index = 161
	{26,	2,	1,	0},    //Freq1 = 972 , Freq2 = 972 , Index = 162
	{26,	2,	1,	0},    //Freq1 = 978 , Freq2 = 972 , Index = 163
	{26,	2,	1,	0},    //Freq1 = 984 , Freq2 = 972 , Index = 164
	{26,	2,	1,	0},    //Freq1 = 990 , Freq2 = 972 , Index = 165
	{26,	2,	1,	0},    //Freq1 = 996 , Freq2 = 972 , Index = 166
	{26,	2,	1,	0},    //Freq1 = 1002, Freq2 = 972 , Index = 167
	{27,	2,	1,	0},    //Freq1 = 1008, Freq2 = 1008, Index = 168
	{27,	2,	1,	0},    //Freq1 = 1014, Freq2 = 1008, Index = 169
	{27,	2,	1,	0},    //Freq1 = 1020, Freq2 = 1008, Index = 170
	{27,	2,	1,	0},    //Freq1 = 1026, Freq2 = 1008, Index = 171
	{27,	2,	1,	0},    //Freq1 = 1032, Freq2 = 1008, Index = 172
	{27,	2,	1,	0},    //Freq1 = 1038, Freq2 = 1008, Index = 173
	{28,	2,	1,	0},    //Freq1 = 1044, Freq2 = 1044, Index = 174
	{28,	2,	1,	0},    //Freq1 = 1050, Freq2 = 1044, Index = 175
	{28,	2,	1,	0},    //Freq1 = 1056, Freq2 = 1044, Index = 176
	{28,	2,	1,	0},    //Freq1 = 1062, Freq2 = 1044, Index = 177
	{28,	2,	1,	0},    //Freq1 = 1068, Freq2 = 1044, Index = 178
	{28,	2,	1,	0},    //Freq1 = 1074, Freq2 = 1044, Index = 179
	{29,	2,	1,	0},    //Freq1 = 1080, Freq2 = 1080, Index = 180
	{29,	2,	1,	0},    //Freq1 = 1086, Freq2 = 1080, Index = 181
	{29,	2,	1,	0},    //Freq1 = 1092, Freq2 = 1080, Index = 182
	{29,	2,	1,	0},    //Freq1 = 1098, Freq2 = 1080, Index = 183
	{29,	2,	1,	0},    //Freq1 = 1104, Freq2 = 1080, Index = 184
	{29,	2,	1,	0},    //Freq1 = 1110, Freq2 = 1080, Index = 185
	{30,	2,	1,	0},    //Freq1 = 1116, Freq2 = 1116, Index = 186
	{30,	2,	1,	0},    //Freq1 = 1122, Freq2 = 1116, Index = 187
	{30,	2,	1,	0},    //Freq1 = 1128, Freq2 = 1116, Index = 188
	{30,	2,	1,	0},    //Freq1 = 1134, Freq2 = 1116, Index = 189
	{30,	2,	1,	0},    //Freq1 = 1140, Freq2 = 1116, Index = 190
	{30,	2,	1,	0},    //Freq1 = 1146, Freq2 = 1116, Index = 191
	{31,	2,	1,	0},    //Freq1 = 1152, Freq2 = 1152, Index = 192
	{31,	2,	1,	0},    //Freq1 = 1158, Freq2 = 1152, Index = 193
	{31,	2,	1,	0},    //Freq1 = 1164, Freq2 = 1152, Index = 194
	{31,	2,	1,	0},    //Freq1 = 1170, Freq2 = 1152, Index = 195
	{31,	2,	1,	0},    //Freq1 = 1176, Freq2 = 1152, Index = 196
	{31,	2,	1,	0},    //Freq1 = 1182, Freq2 = 1152, Index = 197
	{31,	2,	1,	0},    //Freq1 = 1188, Freq2 = 1152, Index = 198
	{31,	2,	1,	0},    //Freq1 = 1194, Freq2 = 1152, Index = 199
	{24,	3,	1,	0},    //Freq1 = 1200, Freq2 = 1200, Index = 200
	{24,	3,	1,	0},    //Freq1 = 1206, Freq2 = 1200, Index = 201
	{24,	3,	1,	0},    //Freq1 = 1212, Freq2 = 1200, Index = 202
	{24,	3,	1,	0},    //Freq1 = 1218, Freq2 = 1200, Index = 203
	{24,	3,	1,	0},    //Freq1 = 1224, Freq2 = 1200, Index = 204
	{24,	3,	1,	0},    //Freq1 = 1230, Freq2 = 1200, Index = 205
	{24,	3,	1,	0},    //Freq1 = 1236, Freq2 = 1200, Index = 206
	{24,	3,	1,	0},    //Freq1 = 1242, Freq2 = 1200, Index = 207
	{25,	3,	1,	0},    //Freq1 = 1248, Freq2 = 1248, Index = 208
	{25,	3,	1,	0},    //Freq1 = 1254, Freq2 = 1248, Index = 209
	{25,	3,	1,	0},    //Freq1 = 1260, Freq2 = 1248, Index = 210
	{25,	3,	1,	0},    //Freq1 = 1266, Freq2 = 1248, Index = 211
	{25,	3,	1,	0},    //Freq1 = 1272, Freq2 = 1248, Index = 212
	{25,	3,	1,	0},    //Freq1 = 1278, Freq2 = 1248, Index = 213
	{25,	3,	1,	0},    //Freq1 = 1284, Freq2 = 1248, Index = 214
	{25,	3,	1,	0},    //Freq1 = 1290, Freq2 = 1248, Index = 215
	{26,	3,	1,	0},    //Freq1 = 1296, Freq2 = 1296, Index = 216
	{26,	3,	1,	0},    //Freq1 = 1302, Freq2 = 1296, Index = 217
	{26,	3,	1,	0},    //Freq1 = 1308, Freq2 = 1296, Index = 218
	{26,	3,	1,	0},    //Freq1 = 1314, Freq2 = 1296, Index = 219
	{26,	3,	1,	0},    //Freq1 = 1320, Freq2 = 1296, Index = 220
	{26,	3,	1,	0},    //Freq1 = 1326, Freq2 = 1296, Index = 221
	{26,	3,	1,	0},    //Freq1 = 1332, Freq2 = 1296, Index = 222
	{26,	3,	1,	0},    //Freq1 = 1338, Freq2 = 1296, Index = 223
	{27,	3,	1,	0},    //Freq1 = 1344, Freq2 = 1344, Index = 224
	{27,	3,	1,	0},    //Freq1 = 1350, Freq2 = 1344, Index = 225
	{27,	3,	1,	0},    //Freq1 = 1356, Freq2 = 1344, Index = 226
	{27,	3,	1,	0},    //Freq1 = 1362, Freq2 = 1344, Index = 227
	{27,	3,	1,	0},    //Freq1 = 1368, Freq2 = 1344, Index = 228
	{27,	3,	1,	0},    //Freq1 = 1374, Freq2 = 1344, Index = 229
	{27,	3,	1,	0},    //Freq1 = 1380, Freq2 = 1344, Index = 230
	{27,	3,	1,	0},    //Freq1 = 1386, Freq2 = 1344, Index = 231
	{28,	3,	1,	0},    //Freq1 = 1392, Freq2 = 1392, Index = 232
	{28,	3,	1,	0},    //Freq1 = 1398, Freq2 = 1392, Index = 233
	{28,	3,	1,	0},    //Freq1 = 1404, Freq2 = 1392, Index = 234
	{28,	3,	1,	0},    //Freq1 = 1410, Freq2 = 1392, Index = 235
	{28,	3,	1,	0},    //Freq1 = 1416, Freq2 = 1392, Index = 236
	{28,	3,	1,	0},    //Freq1 = 1422, Freq2 = 1392, Index = 237
	{28,	3,	1,	0},    //Freq1 = 1428, Freq2 = 1392, Index = 238
	{28,	3,	1,	0},    //Freq1 = 1434, Freq2 = 1392, Index = 239
	{29,	3,	1,	0},    //Freq1 = 1440, Freq2 = 1440, Index = 240
	{29,	3,	1,	0},    //Freq1 = 1446, Freq2 = 1440, Index = 241
	{29,	3,	1,	0},    //Freq1 = 1452, Freq2 = 1440, Index = 242
	{29,	3,	1,	0},    //Freq1 = 1458, Freq2 = 1440, Index = 243
	{29,	3,	1,	0},    //Freq1 = 1464, Freq2 = 1440, Index = 244
	{29,	3,	1,	0},    //Freq1 = 1470, Freq2 = 1440, Index = 245
	{29,	3,	1,	0},    //Freq1 = 1476, Freq2 = 1440, Index = 246
	{29,	3,	1,	0},    //Freq1 = 1482, Freq2 = 1440, Index = 247
	{30,	3,	1,	0},    //Freq1 = 1488, Freq2 = 1488, Index = 248
	{30,	3,	1,	0},    //Freq1 = 1494, Freq2 = 1488, Index = 249
	{30,	3,	1,	0},    //Freq1 = 1500, Freq2 = 1488, Index = 250
	{30,	3,	1,	0},    //Freq1 = 1506, Freq2 = 1488, Index = 251
	{30,	3,	1,	0},    //Freq1 = 1512, Freq2 = 1488, Index = 252
	{30,	3,	1,	0},    //Freq1 = 1518, Freq2 = 1488, Index = 253
	{30,	3,	1,	0},    //Freq1 = 1524, Freq2 = 1488, Index = 254
	{30,	3,	1,	0},    //Freq1 = 1530, Freq2 = 1488, Index = 255
};
static int clk_get_pll_para(struct core_pll_freq_tbl *factor, int rate);
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_corepll(void)
{
	unsigned int reg_val;
	int 	div_m;
	int 	factor_k, factor_n;
	int 	clock;

	reg_val  = readl(CCM_PLL1_CPUX_CTRL);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	div_m    = ((reg_val >> 0) & 0x3) + 1;

	clock = 24 * factor_n * factor_k/div_m;

	return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_axi(void)
{
	int clock;
	unsigned int reg_val;
	int clock_src, factor;

	reg_val   = readl(CCM_CPU_L2_AXI_CTRL);
	clock_src = (reg_val >> 16) & 0x03;
	factor    = (reg_val >> 0) & 0x07;

	if(factor >= 3)
	{
		factor = 4;
	}
	else
	{
		factor ++;
	}

	switch(clock_src)
	{
		case 0:
			clock = 32000;
			break;
		case 1:
			clock = 24;
			break;
		case 2:
			clock =  sunxi_clock_get_corepll();
			break;
		default:
			return 0;
	}

	return clock/factor;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_ahb(void)
{
	unsigned int reg_val;
	int factor;
	int clock;

	reg_val = readl(CCM_AHB1_APB1_CTRL);
	factor  = (reg_val >> 4) & 0x03;
	clock   = sunxi_clock_get_axi()>>factor;

	return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_apb1(void)
{
	unsigned int reg_val;
	int          clock, factor;

	reg_val = readl(CCM_AHB1_APB1_CTRL);
	factor  = (reg_val >> 8) & 0x03;
	clock   = sunxi_clock_get_ahb();

	if(factor)
	{
		clock >>= factor;
	}
	else
	{
		clock >>= 1;
	}

	return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_apb2(void)
{
	return 24;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：只限于调整COREPLL，固定分频比，4:2:1
*
*
************************************************************************************************************
*/
static int clk_get_pll_para(struct core_pll_freq_tbl *factor, int pll_clk)
{
    int                       index;
	struct core_pll_freq_tbl *target_factor;

    index = pll_clk / 6;
	target_factor = &pll1_table[index];

    factor->FactorN = target_factor->FactorN;
    factor->FactorK = target_factor->FactorK;
    factor->FactorM = target_factor->FactorM;

    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int clk_set_divd(int pll)
{
	unsigned int reg_val;

	//config axi
	reg_val = readl(CCM_CPU_L2_AXI_CTRL);
	reg_val &= ~(0x03 << 0);
	reg_val |=  (0x02 << 0);
	writel(reg_val, CCM_CPU_L2_AXI_CTRL);
	//config ahb
	reg_val = readl(CCM_AHB1_APB1_CTRL);;
	reg_val &= ~((0x03 << 12) | (0x03 << 8) | (0x03 << 4));
	reg_val |=   (0x02 << 12);
	if(pll <= 400)
	{
		;
	}
	else if(pll <= 800)
	{
		reg_val |= (1 << 4);
	}
	else if(pll <= 1600)
	{
		reg_val |= (2 << 4);
	}
	else
	{
		reg_val |= (3 << 4);
	}
	writel(reg_val, CCM_AHB1_APB1_CTRL);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：只限于调整COREPLL，固定分频比，4:2:1
*
*
************************************************************************************************************
*/
int sunxi_clock_set_corepll(int frequency, int core_vol)
{
    unsigned int reg_val;
    unsigned int i;
    struct core_pll_freq_tbl  pll_factor;
    //检查时钟是否合法,为0或者超过2G
    if(!frequency)
    {
        //默认频率
        frequency = 408;
    }
    else if(frequency > 1500)
    {
    	frequency = 1500;
    }
    else if(frequency < 24)
    {
		frequency = 24;
    }
    //切换到24M
    reg_val = readl(CCM_CPU_L2_AXI_CTRL);
    reg_val &= ~(0x03 << 16);
    reg_val |=  (0x01 << 16);
    writel(reg_val, CCM_CPU_L2_AXI_CTRL);
    //延时，等待时钟稳定
    for(i=0; i<0x400; i++);
    //调整时钟频率
	clk_get_pll_para(&pll_factor, frequency);
	//回写PLL1
    reg_val = readl(CCM_PLL1_CPUX_CTRL);
    reg_val &= ~((0x1f << 8) | (0x03 << 4) | (0x03 << 0));
	reg_val |=  ((pll_factor.FactorN)<<8) | ((pll_factor.FactorK)<<4) | ((pll_factor.FactorM) << 0);
    writel(reg_val, CCM_PLL1_CPUX_CTRL);
    //延时，等待时钟稳定
	do
	{
		reg_val = readl(CCM_PLL1_CPUX_CTRL);
	}
	while(reg_val & (0x1 << 28));
    //修改AXI,AHB,APB分频
    clk_set_divd(frequency);
    //切换时钟到COREPLL上
    reg_val = readl(CCM_CPU_L2_AXI_CTRL);
    reg_val &= ~(0x03 << 16);
    reg_val |=  (0x02 << 16);
    writel(reg_val, CCM_CPU_L2_AXI_CTRL);

    return  0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_set_pll6(void)
{
    int i;
	unsigned int reg_val;

    /* set voltage and ldo for pll */
    reg_val = readl((SUNXI_R_PRCM_REGS_BASE+0x44));
    reg_val &= ~(0xff << 24);
    reg_val |= 0xa7 << 24;
    writel(reg_val, (SUNXI_R_PRCM_REGS_BASE+0x44));
    reg_val = readl((SUNXI_R_PRCM_REGS_BASE+0x44));
    reg_val &= ~(0x1 << 15);
    reg_val &= ~(0x7 << 16);
    reg_val |= 0x7 << 16;
    writel(reg_val, (SUNXI_R_PRCM_REGS_BASE+0x44));
    /* delaly some time*/
    for(i=0; i<100000; i++);

    /* set pll6 frequency to 600Mhz, and enable it */
	writel(0x80041811, CCM_PLL6_MOD_CTRL);
	do
	{
		reg_val = readl(CCM_PLL6_MOD_CTRL);
	}
	while(!(reg_val & (0x1 << 28)));

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_set_mbus(void)
{
	int factor_n, factor_k, pll6;
	unsigned int reg_val;

    /* set voltage and ldo for pll */
	reg_val = readl(CCM_PLL6_MOD_CTRL);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x03) + 1;
	pll6 = 24 * factor_n * factor_k/2;

    if(pll6 > 300 * 4) {
        factor_n = 5;
    } else if(pll6 > 300*3){
        factor_n = 4;
    } else if(pll6 > 300*2){
        factor_n = 3;
    } else if(pll6 > 300*1){
        factor_n = 2;
    } else {
        factor_n = 1;
    }

    /* config mbus0 */
    writel((0x81000000|(factor_n-1)), CCM_MBUS_SCLK_CTRL0);
    writel((0x81000000|(factor_n-1)), CCM_MBUS_SCLK_CTRL1);

    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_pll6(void)
{
	unsigned int reg_val;
	int factor_n, factor_k, pll6;

	reg_val = readl(CCM_PLL6_MOD_CTRL);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x03) + 1;
	pll6 = 24 * factor_n * factor_k/2;

	return pll6;
}
