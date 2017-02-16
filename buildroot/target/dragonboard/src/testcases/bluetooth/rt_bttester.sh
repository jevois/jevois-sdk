#!/bin/bash
##############################################################################
# \version     1.0.0
# \date        2012年05月31日
# \author      James Deng <csjamesdeng@allwinnertech.com>
# \Descriptions:
#			create the inital version

# \version     1.1.0
# \date        2012年09月26日
# \author      Martin <zhengjiewen@allwinnertech.com>
# \Descriptions:
#			add some new features:
#			1.wifi hotpoint ssid and single strongth san
#			2.sort the hotpoint by single strongth quickly
##############################################################################
source send_cmd_pipe.sh
source script_parser.sh
module_path=`script_fetch "bluetooth" "module_path"`
loop_time=`script_fetch "bluetooth" "test_time"`
destination_bt=`script_fetch "bluetooth" "dst_bt"`
device_node=`script_fetch "bluetooth" "device_node"`
sleep 5


echo $module_path
echo $loop_time
echo "${destination_bt}"
echo $device_node


#insmod "$module_path"
insmod /system/vendor/modules/hci_uart.ko
sleep 2

if [ ! -d /system/etc/ ]; then
	mkdir -p /system/etc
fi

cp /dragonboard/bin/rtl8723b_config /system/etc/
cp /dragonboard/bin/rtl8723b_fw /system/etc/

echo 0 > /sys/class/rfkill/rfkill0/state
sleep 1
echo 1 > /sys/class/rfkill/rfkill0/state
sleep 1

rtk_hciattach -n -s 115200 $device_node rtk_h5 &
#rtk_hciattach -n -s 115200 /dev/ttyS1 rtk_h5
sleep 6

for((i=1;i<=100;i++)); do
echo $i
	rfkillpath="/sys/class/rfkill/rfkill"${i}
	if [ -d "$rfkillpath" ]; then
		if cat $rfkillpath"/type" | grep bluetooth; then
			statepath=${rfkillpath}"/state"
			echo $statepath
			echo 1 > $statepath
			sleep 1
			break
		fi
	fi
done
sleep 3

cciconfig hci0 up
sleep 1
for((i=1;i<=${loop_time} ;i++));  do
	if  hcitool scan  | grep "${destination_bt}" ; then
		 SEND_CMD_PIPE_OK_EX $3 "${destination_bt}"
		# cciconfig hci0 down
		 exit 1
	 fi
   sleep 1
done
cciconfig hci0 down
SEND_CMD_PIPE_FAIL $3
