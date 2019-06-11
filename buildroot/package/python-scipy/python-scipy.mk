################################################################################
#
# python-scipy
#
################################################################################
PYTHON_SCIPY_VERSION = 1.3.0
PYTHON_SCIPY_SOURCE = scipy-$(PYTHON_SCIPY_VERSION).tar.xz
PYTHON_SCIPY_SITE = https://github.com/scipy/scipy/releases/download/v1.3.0
PYTHON_SCIPY_LICENSE = BSD-3-Clause
PYTHON_SCIPY_LICENSE_FILES = LICENSE
PYTHON_SCIPY_ENV = F77=$(HOST_DIR)/bin/arm-buildroot-linux-gnueabihf-gfortran F90=$(HOST_DIR)/bin/arm-buildroot-linux-gnueabihf-gfortran FFLAGS=-fPIC LDFLAGS="-shared -L$(TARGET_DIR)/usr/lib/python$(PYTHON3_VERSION_MAJOR)/site-packages/numpy/core/lib"
PYTHON_SCIPY_SETUP_TYPE = setuptools

PYTHON_SCIPY_DEPENDENCIES = host-python-numpy

ifeq ($(BR2_PACKAGE_CLAPACK),y)
PYTHON_SCIPY_DEPENDENCIES += clapack
PYTHON_SCIPY_SITE_CFG_LIBS += blas lapack
else
PYTHON_SCIPY_ENV += BLAS=None LAPACK=None
endif

define PYTHON_SCIPY_CONFIGURE_CMDS
	-rm -f $(@D)/site.cfg
	echo "[DEFAULT]" >> $(@D)/site.cfg
	echo "library_dirs = $(STAGING_DIR)/usr/lib" >> $(@D)/site.cfg
	echo "include_dirs = $(STAGING_DIR)/usr/include" >> $(@D)/site.cfg
	echo "libraries =" $(subst $(space),$(comma),$(PYTHON_SCIPY_SITE_CFG_LIBS)) >> $(@D)/site.cfg
endef

PYTHON_SCIPY_NEEDS_HOST_PYTHON = python3
PYTHON_SCIPY_INSTALL_STAGING = YES

$(eval $(python-package))

