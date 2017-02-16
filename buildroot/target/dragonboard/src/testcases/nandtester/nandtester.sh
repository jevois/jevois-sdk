#!/bin/sh

source send_cmd_pipe.sh
source script_parser.sh

ROOT_DEVICE=/dev/nandd
for parm in $(cat /proc/cmdline); do
    case $parm in
        root=*)
            ROOT_DEVICE=`echo $parm | awk -F\= '{print $2}'`
            ;;
    esac
done

case $ROOT_DEVICE in
    /dev/nand*)
        echo "nand boot"
        mount /dev/nanda /boot
        SEND_CMD_PIPE_OK_EX $3
        exit 1
        ;;
    /dev/mmc*)
        echo "mmc boot"
        mount /dev/mmcblk0p2 /boot
        ;;
    *)
        echo "default boot type"
        mount /dev/nanda /boot
        ;;
esac
mkfs.vfat /dev/nanda
if [ $? -ne 0 ]; then
echo "format /dev/nanda failed "
    SEND_CMD_PIPE_FAIL_EX $3 "mkfs.vfat"
    exit 1
fi
echo "create vfat file system for /dev/nanda done"

if [ ! -d "/tmp/nanda" ]; then
    mkdir /tmp/nanda
fi

mount /dev/nanda /tmp/nanda
if [ $? -ne 0 ]; then
    SEND_CMD_PIPE_FAIL_EX $3 "mount /dev/nanda"
    exit 1
fi
echo "mount /dev/nanda to /tmp/nanda OK"

#2097152=1024*1024*2
capacity=`cat /sys/block/nand*/size|grep -o '\<[0-9]\{1,\}.[0-9]\{1,\}\>'|awk '{sum+=$0}END{printf "%.2f\n",sum/2097152}'`
let "capacity=$capacity"
echo "nand capacity: $capacity"G""
SEND_CMD_PIPE_MSG $3 $capacity"G"


test_size=`script_fetch "nand" "test_size"`
if [ -z "$test_size" ]; then
    test_size=64
fi

echo "nand test read and write"
if [ $test_size > 0 ]; then
    nandrw "/tmp/nanda/test.bin" "$test_size"
else
    nandrw "/tmp/nanda/test.bin" 64
fi

if [ $? -ne 0 ]; then
    SEND_CMD_PIPE_FAIL_EX $3 "last"
    exit 1
else
    SEND_CMD_PIPE_OK_EX $3 $capacity"G"
fi
