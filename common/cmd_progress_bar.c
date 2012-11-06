#include <common.h>
#include <command.h>
#include <malloc.h>

#define SHOW_OUT_LENGTH		604
#define	SHOW_OUT_WIDTH		104
#define	SHOW_IN_LENGTH		600
#define	SHOW_IN_WIDTH		100
#define	SHOW_LENGTH_MULTI	(600 / 100)
#define	SHOW_LOCATION_X		(800 / 2 - 102)
#define SHOW_LOCATION_Y  	(480 / 2 - 27)
#define SHOW_FRAME_LENGTH	2
#define	SHOW_MEMSIZE		(SHOW_OUT_LENGTH * SHOW_OUT_WIDTH * sizeof(int))
#define SHOW_BITC			32
#define COLOR_RED		0xffff0000
#define COLOR_GREEN		0xff00ff00
#define COLOR_BLUE		0xff0000ff
#define COLOR_YELLOW	0xffffff00
#define COLOR_CYAN		0xff00ffff
#define COLOR_MAGENTA	0xffff00ff
#define COLOR_WHITE		0xffffffff
#define COLOR_BLACK		0xff000000

static int origin_rate;
static void *buf;

/*
************************************************************************************************************
*
* *                                             function
* *
* *    name          :
* *
* *    parmeters     :
* *
* *    return        :
* *
* *    note          : 初始化量产进度指示框
* *                    描绘边框，总共大小 54 * 204, 内部动态区域200 * 50
* *                    边框颜色为红色，内部区域为黑色
* ************************************************************************************************************
* */
int sunxi_sprite_rate_init(void)
{
	unsigned int	*buff;
	int i;
	buf		= malloc(SHOW_MEMSIZE);
	buff	= (unsigned int *)buf;
	for(i=0; i<SHOW_MEMSIZE / sizeof(int); i++){
		if((i / SHOW_OUT_LENGTH < SHOW_FRAME_LENGTH)
			|| (i / SHOW_OUT_LENGTH >= SHOW_OUT_WIDTH - SHOW_FRAME_LENGTH)
			|| (i % SHOW_OUT_LENGTH < SHOW_FRAME_LENGTH)
			||(i % SHOW_OUT_LENGTH >= SHOW_OUT_LENGTH -SHOW_FRAME_LENGTH)){
		*(buff + i)	= COLOR_RED;
		}else{
		*(buff + i)	= COLOR_BLACK;
		}
	}
	board_display_framebuffer_set(SHOW_OUT_LENGTH, SHOW_OUT_WIDTH,SHOW_BITC,buf);
	board_display_layer_show(0);
	origin_rate	= 0;
	return 0;
}
/*
 * ************************************************************************************************************
 * *
 * *                                             function
 * *
 * *    name          :
 * *
 * *    parmeters     :
 * *
 * *    return        :
 * *
 * *    note          :  根据传进的rate，从左边开始计算，把2*rate的区域染成绿色，其它部分保持黑色
 * *                     边框颜色保持不变
 * *
 * ************************************************************************************************************
 * */

int sunxi_sprite_rate_display(int rate)
{
	unsigned int    *buff;
	int i,j,k;
	buff=buf;
	if(rate > 100 || rate < 0 || rate < origin_rate)
		return -1;
	/*
	for(i=0; i<SHOW_MEMSIZE / sizeof(int); i++){
		if((i >= SHOW_FRAME_LENGTH * SHOW_OUT_LENGTH) 
			&& (i < (SHOW_OUT_WIDTH - SHOW_FRAME_LENGTH) * SHOW_OUT_LENGTH)
			&& (i%SHOW_OUT_LENGTH > SHOW_FRAME_LENGTH + origin_rate*6) 
			&& (i%SHOW_OUT_LENGTH <= SHOW_FRAME_LENGTH + rate*6)){

			*(buff + i) = COLOR_GREEN;
		}
	}
	*/
	for(i=SHOW_FRAME_LENGTH;i< SHOW_OUT_WIDTH - SHOW_FRAME_LENGTH;i++)
		for(j=origin_rate;j<rate;j++)
			for(k=0;k<SHOW_LENGTH_MULTI;k++){
				*(buff + i*SHOW_OUT_LENGTH + j*SHOW_LENGTH_MULTI + k + SHOW_FRAME_LENGTH)= COLOR_GREEN;
	}
	origin_rate	= rate;
	return 0;
}
/*
 * ************************************************************************************************************
 * *
 * *                                             function
 * *
 * *    name          :
 * *
 * *    parmeters     :
 * *
 * *    return        :
 * *
 * *    note          :  可以不执行任务，直接返回
 * *
 * *
 * ************************************************************************************************************
 * */
int sunxi_sprite_rate_exit(void)
{
	return 0;
}

static int do_sunxi_progress_bar_display(cmd_tbl_t * cmdtp,int flag,int argc,char *const argv[])
{
	int		i,rate;

	rate = (int)simple_strtoul(argv[1], NULL, 10);
	printf("rate is %d\n",rate);
	if(rate && rate <= 100 && rate >=0){
		sunxi_sprite_rate_display(rate);
	}else if(rate){
		sunxi_sprite_rate_init();
		for(i=0;i<=100;i++){
			__msdelay(100);
			sunxi_sprite_rate_display(i);
		}
	}else{
		sunxi_sprite_rate_init();
	}

	return 0;
}


U_BOOT_CMD(
	progress_bar,	2,	0,	do_sunxi_progress_bar_display,
	"progress_bar test",
	"arg1: 0 progress_bar init other sunxi_sprite_rate_display\n"
	"arg1: only support from 0 to 100\n"
);


