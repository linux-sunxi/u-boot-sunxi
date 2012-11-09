/*
 * BMP handling routines
 */

#include <common.h>
#include <bmp_layout.h>
#include <command.h>
#include <malloc.h>

#define	SUNXI_WAV_RIFF	0X46464952
#define	SUNXI_WAV_WAVE	0X45564157
#define	SUNXI_WAV_FMT	0x20746d66
#define	SUNXI_WAV_FACT	0x74636166
#define	SUNXI_WAV_DATA	0x61746164
typedef struct{
	unsigned int	riff_id;//value is "riff"
	unsigned int	size;
	unsigned int	riff_format;//value is wave
}
riffwave_chunk_t;

typedef struct{
	short    int	format_tag;
	short    int	channels;
	unsigned int	samplespersec;
	unsigned int	avgbytespersec;
	short    int	block_align;
	short    int	bitspersample;
}
wave_format_t;

typedef struct{
	unsigned int	fmt_id;
	unsigned int	size;
	wave_format_t	wave_format;
}
format_chunk_t;

typedef struct{
	unsigned int	fact_id;
	unsigned int	size;
	unsigned int	fact_data;
}
fact_chunk_t;

typedef struct{
	unsigned int	data_id;
	unsigned int	size;
}
data_chunk_t;

typedef struct{
	riffwave_chunk_t	riff_wave;
	format_chunk_t		format;
	fact_chunk_t		fact;
	data_chunk_t		data;
}
sunxi_wav_t;

typedef struct{
	unsigned int		id;
	unsigned int		size;
}
tmp_chunk_t;

typedef struct{
	unsigned int	channels;
	unsigned int	wbitspersmple;
	unsigned int	nsamplespersec;
	unsigned int	data_size;
	char			*data;
}
sunxi_wav_infoi_t;

static int sunxi_parse_wav(unsigned int addr,sunxi_wav_infoi_t *sunxi_wav_info)
{
	riffwave_chunk_t	*riffwave_chunk;
	tmp_chunk_t			*tmp_chunk;
	format_chunk_t		*format_chunk;
	data_chunk_t		*data_chunk;
	int					fmt_and_data=0;
	char				*p;
	if(!addr){
		printf("have no availed buff!\n");	
		return -1;
	}else{
		riffwave_chunk	= (riffwave_chunk_t *)addr;
		p				= (char *)addr;
	}

	printf("sunxi_parse_wav p 0x%x\n",p);
	if((riffwave_chunk->riff_id == SUNXI_WAV_RIFF)&&(riffwave_chunk->riff_format == SUNXI_WAV_WAVE)){
		p 			+= 12;
		tmp_chunk	= (tmp_chunk_t *)p;
		while(fmt_and_data<2){
			switch(tmp_chunk->id){
				case SUNXI_WAV_FMT:{
					format_chunk	= (format_chunk_t *)p;
					sunxi_wav_info->channels		= format_chunk->wave_format.channels;
					sunxi_wav_info->wbitspersmple	= format_chunk->wave_format.bitspersample;
					sunxi_wav_info->nsamplespersec	= format_chunk->wave_format.samplespersec;
					p	+= (8 + tmp_chunk->size);
					tmp_chunk		= (tmp_chunk_t *)p;
					fmt_and_data++;
					break;
				}
				case SUNXI_WAV_DATA:{
					data_chunk		= (data_chunk_t *)p;
					sunxi_wav_info->data_size		= data_chunk->size;
					sunxi_wav_info->data			= p+8;
					p   += (8 + tmp_chunk->size);
					tmp_chunk		= (tmp_chunk_t *)p;
					fmt_and_data++;
					break;
				}
				case SUNXI_WAV_FACT:{
					p   += (8 + tmp_chunk->size);
					tmp_chunk       = (tmp_chunk_t *)p;
					break;
				}
				default:{
					printf("data err!\n");		
				}
			}
		}
	printf("channels %u wbitspersmple %u nsamplespersec %u data_size %u data 0x%x\n",sunxi_wav_info->channels,
			sunxi_wav_info->wbitspersmple,sunxi_wav_info->nsamplespersec,sunxi_wav_info->data_size,sunxi_wav_info->data);	
	}else{
		printf("file format is not wav!\n");
		return 1;
	}
	return 0;
}
/*
static int copy_wav(char *load_addr,char *filename){
	char *const bmp_argv[6] = { "fatload", "sunxi_flash", "0:1", load_addr, filename, NULL };
	if(do_fat_fsload(0, 0, 5, bmp_argv)){
		printf("do_fat_fsload err!\n");
		return 1;
	}
	return 0;
}
*/
char  	sunxi_load_addr[8];
char	filename[32];
char	*const wav_argv[6] = { "fatload", "sunxi_flash", "0:1", sunxi_load_addr, filename, NULL };
	
static int do_sunxi_wav(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int	addr;
	int				ret=0;
	char			*buf=NULL;
	sunxi_wav_infoi_t sunxi_wav_info;
	int i;


	/* use argument only*/
	addr = simple_strtoul(argv[1], NULL, 16);
	memset(filename, 0, 32);
	memcpy(filename, argv[2], strlen(argv[2]));
	debug("wav addr=%x\n", addr);
	if(!addr){
		buf		= (char *)malloc(2*1024*1024);
		if(!buf){
			printf("malloc fail!\n");
			return 1;
		}
		addr	= (unsigned int)buf;
		printf("buf 0x%x addr 0x%x",buf,addr);
	}
	sprintf(sunxi_load_addr,"%x",addr);
/*
	printf("load_addr:");
	for(i=0;i<8;i++)
		printf("%c",load_addr[i]);
	printf("\n");
*/
	if(do_fat_fsload(0, 0, 5, wav_argv)){
		printf("do_fat_fsload err!\n");
		return 1;
	}

	ret = sunxi_parse_wav(addr,&sunxi_wav_info);
	if(buf){
		free(buf);
	}	
	printf("out channels %u wbitspersmple %u nsamplespersec %u data_size %u data 0x%x\n",sunxi_wav_info.channels,
			sunxi_wav_info.wbitspersmple,sunxi_wav_info.nsamplespersec,sunxi_wav_info.data_size,sunxi_wav_info.data);
	return 0;
}

U_BOOT_CMD(
	sunxi_wav,	3,	0,	do_sunxi_wav,
	"parse wav file data",
	"parse wav file info\n"
);


