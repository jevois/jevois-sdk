#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

-include hardware/broadcom/wlan/bcmdhd/config/config-bcm.mk

WIFI_DRIVER_FW_PATH_STA    := "/system/vendor/modules/fw_bcm40181a2.bin"
WIFI_DRIVER_FW_PATH_P2P    := "/system/vendor/modules/fw_bcm40181a2_p2p.bin"
WIFI_DRIVER_FW_PATH_AP     := "/system/vendor/modules/fw_bcm40181a2_apsta.bin"

PRODUCT_COPY_FILES += \
    hardware/broadcom/wlan/firmware/ap6476/fw_bcm40181a2.bin:system/vendor/modules/fw_bcm40181a2.bin \
    hardware/broadcom/wlan/firmware/ap6476/fw_bcm40181a2_apsta.bin:system/vendor/modules/fw_bcm40181a2_apsta.bin \
    hardware/broadcom/wlan/firmware/ap6476/fw_bcm40181a2_p2p.bin:system/vendor/modules/fw_bcm40181a2_p2p.bin \
    hardware/broadcom/wlan/firmware/ap6476/nvram_ap6476.txt:system/vendor/modules/nvram_ap6476.txt \
    hardware/broadcom/wlan/firmware/ap6476/bcm2076b1.hcd:system/vendor/modules/bcm2076b1.hcd \
    hardware/broadcom/wlan/firmware/ap6476/config.txt:system/vendor/modules/config.txt
