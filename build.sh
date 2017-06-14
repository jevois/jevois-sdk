#!/bin/bash

set -e

buildroot/scripts/mkcommon.sh $@

# JEVOIS: fix arisc.fex location

cp tools/out_sun8iw5p1_linux_common_arisc out/sun8iw5p1/linux/common/arisc
