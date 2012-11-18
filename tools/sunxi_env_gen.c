/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Enviroment partition generator for Allwinner A10 platform.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <config.h>
#include <environment.h>

extern uint32_t crc32 (uint32_t, const unsigned char *, uint);

int main(int argc, char * argv[])
{
	int fin, fout;
	int i, j,ret,count= 0,flag=0;
	char tmp[ENV_SIZE];
	
	env_t env;

	size_t len = 0;

	if(argc != 3) {
		printf("This program generate the u-boot env partition from config file\n");
		printf("Usage: %s input_config_file out_put_bin_file\n", argv[0]);
		return 0;
	}

	fin = open(argv[1], O_RDONLY);
	fout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);

	if(fin < 0) {
		printf("%s ", argv[1]);
		perror("open error");
		return -1;
	}

	if(fout < 0) {
		printf("%s ", argv[2]);
		perror("can not create file");
		return -1;
	}
	/* take the file size */
	len = lseek(fin, 0, SEEK_END);
	
	lseek(fin, 0, SEEK_SET);
	if( read(fin, env.data, len) < 0 ) {
		printf("read failed");
		return -1;
	}
	for (i=0;i<len;i++){
		if(env.data[i] == '#'){
		flag=1;
	
		}
		if(env.data[i] == '\n'){
		flag=0;
		
	
		}
		if(flag == 0 ){
		tmp[count++]=env.data[i];
	}
	}

	memset(env.data,0,len);
	
	
	
#ifdef DEBUG
	printf("len: %d\n", len);
#endif


/*
	if(len > ENV_SIZE)
	{
		printf("Warning: %s length is larger than CONFIG_ENV_SIZE\n", argv[1]);
		len = ENV_SIZE;
	}

	lseek(fin, 0, SEEK_SET);

	if( read(fin, env.data, len) < 0 ) {
		printf("read failed");
		return -1;
	}
	
*/
#ifdef DEBUG
	printf("buf:\n%s\n", env.data);
#endif
	for(i = 0,j = 0; i < ENV_SIZE; i++ ,j++) {
		
		if(tmp[i] == '\n'){
		env.data[j] = '\0';
	
		}
		else  env.data[j] =tmp[i] ;
		
		if ((tmp[i] == '\n') && (tmp[i-1] == '\n'))
		{
			j--;
		}
	}
	

	
	
	
	env.crc = crc32(0, env.data, ENV_SIZE);
#ifdef DEBUG
	printf("env.crc:%x\n", env.crc);
#endif	
	ret = write(fout, &env, CONFIG_ENV_SIZE);

	if(ret < 1) {
		printf("Write error, create env image failed\n");
	}

	close(fin);
	close(fout);

	return 0;
}
