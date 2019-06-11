################################################################################
#
# python-pylibdmtx
#
################################################################################

PYTHON_PYLIBDMTX_VERSION = 0.1.9
PYTHON_PYLIBDMTX_SITE = https://github.com/NaturalHistoryMuseum/pylibdmtx/archive/v$(PYTHON_PYLIBDMTX_VERSION)
PYTHON_PYLIBDMTX_SETUP_TYPE = setuptools
PYTHON_PYLIBDMTX_LICENSE = MIT
PYTHON_PYLIBDMTX_LICENSE_FILES = LICENSE.txt

$(eval $(python-package))
