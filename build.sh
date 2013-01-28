# build.sh
#
# (c) Copyright 2013
# Allwinner Technology Co., Ltd. <www.allwinnertech.com>
# James Deng <csjamesdeng@allwinnertech.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# Notice:
#   1. Board option is useless.
#   2. We Only support to copy u-boot.bin to common directory right now.

function build_uboot()
{
    case $1 in
        clean)
            make distclean CROSS_COMPILE=arm-linux-gnueabi-
            ;;
        *)
            make -j8 ${LICHEE_CHIP} CROSS_COMPILE=arm-linux-gnueabi-
            cp -f u-boot.bin ../out/${LICHEE_CHIP}/${LICHEE_PLATFORM}/common/
            ;;
    esac
}

if [ $(basename `pwd`) != "u-boot" ] ; then
    echo "Please run at the top directory of u-boot"
    exit 1
fi

if [ -n "${LICHEE_CHIP}" ] ; then
    build_uboot $1
    exit
fi

. ../buildroot/scripts/shflags/shflags

# define option, format:
#   'long option' 'default value' 'help message' 'short option'
DEFINE_string 'platform' 'sun7i' 'platform to build, e.g. sun7i' 'p'
DEFINE_string 'board' '' 'board to build, e.g. evb' 'b'
DEFINE_string 'module' '' 'module to build, e.g. clean' 'm'

# parse the command-line
FLAGS "$@" || exit $?
eval set -- "${FLAGS_ARGV}"

LICHEE_CHIP=${FLAGS_platform%%_*}
LICHEE_PLATFORM=${FLAGS_platform##*_}
LICHEE_BOARD=${FLAGS_board}

if [ "${LICHEE_CHIP}" = "${LICHEE_PLATFORM}" ] ; then
    LICHEE_PLATFORM="linux"
fi

tooldir="$(dirname `pwd`)/out/${LICHEE_CHIP}/${LICHEE_PLATFORM}/common/buildroot/external-toolchain"
if [ -d ${tooldir} ] ; then
    if ! echo $PATH | grep "$tooldir" ; then
        export PATH=${tooldir}/bin:$PATH
    fi
else
    echo "Please build buildroot first"
    exit 1
fi

build_uboot ${FLAGS_module}
