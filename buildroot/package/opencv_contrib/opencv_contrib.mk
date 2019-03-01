################################################################################
#
# opencvcontrib
#
################################################################################

OPENCV_CONTRIB_VERSION = 4.0.1
OPENCV_CONTRIB_SITE = $(call github,opencv,opencv_contrib,$(OPENCV_CONTRIB_VERSION))
OPENCV_CONTRIB_INSTALL_STAGING = YES
OPENCV_CONTRIB_LICENSE = BSD-3c
OPENCV_CONTRIB_LICENSE_FILES = LICENSE

# nothing to build, this package is built as part of building opencv3
define OPENCV_CONTRIB_BUILD_CMDS
endef

define OPENCV_CONTRIB_INSTALL_STAGING_CMDS
endef

define OPENCV_CONTRIB_INSTALL_TARGET_CMDS
endef

$(eval $(generic-package))
