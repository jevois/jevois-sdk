################################################################################
#
# python-pytorch
#
################################################################################

PYTHON_PYTORCH_VERSION = v1.1.0
PYTHON_PYTORCH_SITE = https://github.com/pytorch/pytorch
PYTHON_PYTORCH_SITE_METHOD = git
PYTHON_PYTORCH_GIT_SUBMODULES = YES
PYTHON_PYTORCH_SETUP_TYPE = setuptools
PYTHON_PYTORCH_LICENSE = MIT
PYTHON_PYTORCH_LICENSE_FILES = LICENSE.txt
PYTHON_PYTORCH_DEPENDENCIES = host-python-pyyaml

PYTHON_PYTORCH_ENV += MAX_JOBS=32 NO_CUDA=1 NO_DISTRIBUTED=1 NO_MKLDNN=1 USE_NNPACK=1 USE_QNNPACK=1 USE_NUMPY=1 USE_OPENCV=1 USE_SYSTEM_EIGEN_INSTALL=1 USE_CFLAGS=1

ifeq ($(BR2_PACKAGE_CLAPACK),y)
PYTHON_PYTORCH_DEPENDENCIES += clapack
PYTHON_PYTORCH_SITE_CFG_LIBS += blas lapack
else
##PYTHON_PYTORCH_ENV += BLAS=None LAPACK=None
endif

define PYTHON_PYTORCH_CONFIGURE_CMDS
	-rm -f $(@D)/site.cfg
	echo "[DEFAULT]" >> $(@D)/site.cfg
	echo "library_dirs = $(STAGING_DIR)/usr/lib" >> $(@D)/site.cfg
	echo "include_dirs = $(STAGING_DIR)/usr/include" >> $(@D)/site.cfg
	echo "libraries =" $(subst $(space),$(comma),$(PYTHON_PYTORCH_SITE_CFG_LIBS)) >> $(@D)/site.cfg
endef

PYTHON_PYTORCH_NEEDS_HOST_PYTHON = python3

$(eval $(python-package))
$(eval $(host-python-package))
