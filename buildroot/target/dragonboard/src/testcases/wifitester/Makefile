
# define sources root directory before everything
SRC_ROOT := ../..

# include rule.mk
include $(SRC_ROOT)/rule.mk

# add you test case here
.PHONY: all
all:
	#make -C stp_uart_launcher
	#make -C write_devnode
	cp wifitester.sh $(BINDIR)/
	cp rt_wifitester.sh $(BINDIR)/	
	cp *.bin $(BINDIR)/
	cp *.txt $(BINDIR)/
	cp *.hcd $(BINDIR)/