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

BOOT_TYPE=-1
for parm in $(cat /proc/cmdline); do
    case $parm in
        boot_type=*)
            BOOT_TYPE=`echo $parm | awk -F\= '{print $2}'`
            ;;
    esac
done

case $ROOT_DEVICE in
    /dev/nand*)
        echo "nand boot"
        mount /dev/nanda /boot
        ;;
    /dev/mmc*)
        echo "mmc boot"
        case $BOOT_TYPE in
            2)
                echo "boot_type = 2"
                mount /dev/mmcblk0p2 /boot
                SEND_CMD_PIPE_OK_EX $3
                exit 1
                ;;
            *)
                echo "boot_type = $BOOT_TYPE"
                mount /dev/mmcblk0p2 /boot
                ;;
        esac
        ;;
    *)
        echo "default boot type"
        mount /dev/nanda /boot
        ;;
esac

flashdev="/dev/mmcblk"
mountpoint="/tmp/flash"

if [ ! -d "/sys/devices/platform/sunxi-mmc.2" ]; then
	SEND_CMD_PIPE_FAIL $3
    exit 1
fi


cd "/sys/devices/platform/sunxi-mmc.2/mmc_host/"; a=`ls`;cd -
devindex=${a:3:1}
flashdev=$flashdev$devindex
echo "flashdev=$flashdev"

mkfs.vfat $flashdev
if [ $? -ne 0 ]; then
    SEND_CMD_PIPE_FAIL $3
    exit 1
fi
echo "create vfat file system for /dev/nanda done"

if [ ! -d $mountpoint ]; then
    mkdir $mountpoint
fi

mount $flashdev $mountpoint
if [ $? -ne 0 ]; then
    SEND_CMD_PIPE_FAIL $3
    exit 1
fi
echo "mount $flashdev to $mountpoint OK"

capacity=`df -h | grep $flashdev | awk '{printf $2}'`
echo "flash capacity: $capacity"
SEND_CMD_PIPE_MSG $3 $capacity

total_size=`busybox df -m | grep $flashdev | awk '{printf $2}'`
echo "total_size=$total_size"
test_size=`script_fetch "emmc" "test_size"`
if [ -z "$test_size" -o $test_size -le 0 -o $test_size -gt $total_size ]; then
    test_size=64
fi

echo "test_size=$test_size"
echo "emmc test read and write"
emmcrw "$mountpoint/test.bin" "$test_size"
if [ $? -ne 0 ]; then
    SEND_CMD_PIPE_FAIL $3
else
    SEND_CMD_PIPE_OK_EX $3 $capacity
fi
