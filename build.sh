#!/bin/bash
set -e

PLATFORM="sun6i"
MODULE=""
TOOLSPATH=`pwd`
show_help()
{
	printf "\nbuild.sh - Top level build scritps\n"
	echo "Valid Options:"
	echo "  -h  Show help message"
	echo "  -p <platform> platform, e.g. sun4i, sun4i-lite, sun4i_crane"
	printf "  -m <module> module\n\n"
}

while getopts hp:m: OPTION
do
	case $OPTION in
	h) show_help
	;;
	p) PLATFORM=$OPTARG
	;;
	m) MODULE=$OPTARG
	;;
	*) show_help
	;;
esac
done

if [ -z "$PLATFORM" ]; then
	show_help
	exit 1
fi

if [ -z "$MODULE" ]; then
	MODULE="all"
fi

	export PATH=$PATH:${TOOLSPATH}/../buildroot/output/external-toolchain/bin/
	make distclean CROSS_COMPILE=arm-linux-gnueabi- && make -j8 $PLATFORM CROSS_COMPILE=arm-linux-gnueabi-
	cp -rf u-boot.bin  ../brandy/out_source
