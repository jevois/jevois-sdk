#!/bin/bash
set -e

PLATFORM="sun8iw3p1"

show_help()
{
	printf "\nbuild.sh - Top level build scritps\n"
	echo "Valid Options:"
	echo "  -h  Show help message"
	echo "  -p <platform> platform, e.g. sun5i, sun6i, sun8iw1p1, sun8iw3p1, sun9iw1p1"
	printf "\n\n"
}

build_uboot()
{
	cd u-boot-2011.09/
	make distclean
	make ${PLATFORM}_config
	make -j16


	cd - 1>/dev/null
}

while getopts p: OPTION
do
	case $OPTION in
	p) PLATFORM=$OPTARG
	;;
	*) show_help
	;;
esac
done


build_uboot




