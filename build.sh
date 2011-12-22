#!/bin/bash
set -e

PLATFORM=""
MODULE=""

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

if [ "$PLATFORM" = "sun4i_crane" ]; then
	make distclean && make -j4 sun4i CROSS_COMPILE=arm-none-linux-gnueabi-
else
	make distclean && make -j4 $PLATFORM CROSS_COMPILE=arm-none-linux-gnueabi-
fi
