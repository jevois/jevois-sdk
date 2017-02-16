# define sources root directory before everything
SRC_ROOT := ../..

# change compiler and linker option before you include rule.mk
#
# link to libscript.a when you need to fetch configuration
# from test_script
#
#CFLAGS := $(CFLAGS) -g
LDFLAGS := -lscript

# include rule.mk
include $(SRC_ROOT)/rule.mk

# define objects collection variable
rtk_hciattach_objs = hciattach.o hciattach_rtk.o

# add your target(s) to all
.PHONY: all
all: rtk_hciattach

# define you target, the target will be output to dragonboard/output/bin
# directory
rtk_hciattach: $(rtk_hciattach_objs)
	$(LINK_MSG)
	$(LINKX)

# change 'rtk_hciattach_objs' to your objects collection variable
$(rtk_hciattach_objs): %.o: %.c
	$(COMPILE_MSG)
	$(COMPILEX)

