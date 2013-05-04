if you want marsboard boot more faster and stable.

modified as follows:

u-boot-sunxi\common\main.c  main_loop() function.

bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

change to 

bootdelay = 0;
