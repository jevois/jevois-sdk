#!/bin/bash
# USAGE: jevois-flash-card.sh [-y] [device]
# If -y is specified, assume all defaults are ok

echo "JeVois microSD card builder. Copyright (C) 2017 by Laurent Itti, iLab and USC."
echo

usedef=0
card=$1

if [ "x$card" = "x-y" ]; then
    usedef=1
    card=$2
fi

if [ "x$card" = "x" ]; then
    read -p "Enter device name for microSD card: ";
    if [ "X$REPLY" != "X" ]; then card=$REPLY; fi
else
    if [ $usedef -ne 1 ]; then
	read -p "Enter device name for microSD card [default $1]: "
	if [ "X$REPLY" != "X" ]; then card=$REPLY; fi
    fi
fi

if [ "x$card" = "x" ]; then echo "Cannot write to empty card device -- ABORT"; exit 2; fi
if [ ! -b "$card" ]; then echo "Specified card device [$card] not a block device -- ABORT"; exit 3; fi

echo
echo "=============================== Contents of $card:"
echo abort | sfdisk --no-reread --force ${card} | grep -v Checking | grep -v Leaving.
echo "=================================================="
echo
echo
if [ $usedef -ne 1 ]; then
    read -p "Ok to ERASE ALL DATA on $card? [y/N] "
    if [ "X$REPLY" != "Xy" ]; then echo "Operation aborted by user."; exit 2; fi
fi

# Make sure the card was not automatically mounted:
umount ${card}* > /dev/null 2>&1
sleep 3

# Nuke any old partition table and bootloader:
echo
echo "Erasing old partitions and partition table..."
dd if=/dev/zero of=${card} bs=1M count=100
sync

echo "Flashing boot0 loader..."
dd if=boot0_sdcard.fex of=${card} bs=1k seek=8
sync

echo "Flashing u-boot loader..."
dd if=u-boot.fex of=${card} bs=1k seek=19096
sync

# Units for sfdisk are sectors; u-boot starts at 38192 and is about 510k, so start partitions after sector 39200.
# Here we first define desired partition sizes in MEGABYTES to make it simple:
# p1 is vfat and is boot partition
# p2 is ext4 and is root
# p3 is vfat and is JeVois data, fills entire card
p1siz=16
if [ $usedef -ne 1 ]; then
    read -p "Enter size in MB for boot partition [${p1siz}] "
    if [ "X$REPLY" != "X" ]; then p1siz=$REPLY; fi
fi

p2siz=1024
if [ $usedef -ne 1 ]; then
    read -p "Enter size in MB for Linux system partition [${p2siz}] "
    if [ "X$REPLY" != "X" ]; then p2siz=$REPLY; fi
fi

echo "Creating partitions on ${card} ..."

# Compute all offsets and sizes in sectors of 512 bytes:
p1start=39936
p1sec=`expr ${p1siz} \* 2048`
p2start=`expr ${p1start} + ${p1sec}`
p2sec=`expr ${p2siz} \* 2048`
p3start=`expr ${p2start} + ${p2sec}`
echo ${p1start},${p1sec},c
echo ${p2start},${p2sec},L
echo ${p3start},,c

# Write new partition table:
cat <<EOT | sfdisk ${card}
${p1start},${p1sec},c
${p2start},${p2sec},L
${p3start},,c
EOT
sync
sleep 2

p=""
if [[ -b "${card}p1" ]]; then p="p"; fi

echo "Creating filesystems on ${card} ..."
mkfs.vfat -n BOOT ${card}${p}1
sync

# For future reference, because we run kernel 3.4 on the platform, we need
# to make sure we do not here create the fs with unsupported features. These
# features are ok, disable any extra ones:
#
# sudo tune2fs -l /dev/sdb2 
# Filesystem features: has_journal ext_attr resize_inode dir_index filetype
# needs_recovery extent flex_bg sparse_super large_file huge_file uninit_bg
# dir_nlink extra_isize
mkfs.ext4 -L LINUX -O ^64bit,uninit_bg,^metadata_csum ${card}${p}2
sync
mkfs.vfat -n JEVOIS ${card}${p}3
sync
sleep 2

mkdir mnt-jevois
mount ${card}${p}2 mnt-jevois
mkdir mnt-jevois/boot mnt-jevois/jevois
mount ${card}${p}1 mnt-jevois/boot
mount ${card}${p}3 mnt-jevois/jevois
mkdir mnt-jevois/jevois/data mnt-jevois/jevois/lib mnt-jevois/jevois/modules  mnt-jevois/jevois/packages

echo "Populating boot partition ${card}1 ..."
cp uImage script.bin uEnv.txt mnt-jevois/boot/

if [ $usedef -ne 1 ]; then
    read -p "Drop to login prompt instead of starting JeVois software automatically [y/N] "
    if [ "X$REPLY" = "Xy" ]; then touch mnt-jevois/boot/login; fi

    read -p "Disable serial-over-USB port [y/N] "
    if [ "X$REPLY" = "Xy" ]; then touch mnt-jevois/boot/nousbserial; fi
fi
sync

echo "Populating root partition ${card}2 and jevois partition ${card}3 ..."

mkdir rootfs-jevois
mount -o loop rootfs.ext4 rootfs-jevois
cp -arv rootfs-jevois/* mnt-jevois/
sync
umount rootfs-jevois
/bin/rm -rf rootfs-jevois

# patch up the rootfs:
cat > mnt-jevois/etc/fstab <<EOF
/dev/root	/		ext4	rw,noauto	0	0
devpts		/dev/pts	devpts	defaults,gid=5,mode=620	0	0
tmpfs		/dev/shm	tmpfs	mode=0777	0	0
tmpfs		/tmp		tmpfs	mode=1777	0	0
tmpfs		/run		tmpfs	mode=0755,nosuid,nodev	0	0
sysfs		/sys		sysfs	defaults	0	0
debugfs         /sys/kernel/debug      debugfs  defaults  0 0
/dev/mmcblk0p1  /boot           vfat    defaults 0 0
/dev/mmcblk0p3  /jevois         vfat    defaults 0 0
EOF

cat > mnt-jevois/etc/init.d/rcS <<EOF
#!/bin/sh
hostname jevois
MODULES_DIR=/lib/modules/3.4.39

if [ ! -f /boot/login ]; then /usr/bin/jevois.sh; fi
EOF

# Do not activate a console on ttyS0 by default, since we use it for communication with Arduino:
if [ ! -f mnt-jevois/boot/login ]; then
    sed -i s/ttyS0/ttyS1/ mnt-jevois/etc/inittab
fi

echo "Flushing caches and releasing card..."

sync
umount mnt-jevois/jevois
umount mnt-jevois/boot
umount mnt-jevois
sync

/bin/rm -rf mnt-jevois

echo "microSD card is now loaded with JeVois software."
echo "microSD card is ready to be inserted into your JeVois smart camera."
