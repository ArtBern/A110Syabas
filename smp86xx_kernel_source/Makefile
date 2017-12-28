#
# This is the Sigma Designs Makefile for the preparation of the MIPS kernel 
# source for the SMP86xx family of chips.
#
# Copyright (c) 2005 Sigma Designs, Inc <http://www.sigmadesigns.com>
#
SHELL = /bin/sh
.SUFFIXES:

ifndef $(PWD)
PWD := $(shell pwd)
endif

patch_dir := pf/patches
current_dir := $(shell basename $(PWD))

wget          := wget --passive-ftp
download_site := ftp://armutils:sigmadesigns@gw.sigmadesigns.com/smp86xx


all:
	@echo
	@echo "Linux kernel; Sigma Designs release is $(RELEASE_NAME)"
	@echo
	@echo "Available targets:"
	@echo "  kernel-source-2.4.30: prepares the kernel source under ./linuxmips-2.4.30"
	@echo "  kernel-source-2.6.15: prepares the kernel source under ./linux-2.6.15"
	@echo "  superclean          : cleans all created files away"
	@echo
	@echo "Once you have prepared the kernel source, you can build the"
	@echo "Linux kernel in a regular manner (make menuconfig, make dep,"
	@echo "make). Your end target is vmlinux.bin under the kernel base"
	@echo "directory".
	@echo

.PHONY: superclean
superclean:
	-rm $(kernel_src_2_4_30)
	-rm $(kernel_src_2_6_15)
	-rm -r $(kernel_src_2_4_30_dir)
	-rm -r $(kernel_src_2_6_15_dir)

###############################################################################
#
# Kernel 2.4.30
#
###############################################################################

kernel_src_2_4_30           := linuxmips-2.4.30.tar.bz2
kernel_src_2_4_30_dir       := linuxmips-2.4.30
kernel_2_4_30_config        := config-2.4.30-smp86xx

$(kernel_src_2_4_30_dir)/.unpacked: $(kernel_src_2_4_30)
.PHONY: kernel-source-2.4.30-unpacked
kernel-source-2.4.30-unpacked: $(kernel_src_2_4_30_dir)/.unpacked

patch_list_2_4_30 := \
    0000.shared.patch \
    1000.tango2.patch \
    1002.vectors.patch \
    1003.uart.patch \
    1004.scsi.patch \
    1005.sbox-mbus-gpio.patch \
    1006.ide.patch \
    1007.ir.patch \
    1008.fip.patch \
    1009.mtd.patch \
    1010.pci.patch \
    1011.udf.patch \
    1012.ccache.patch \
    1013.zbf.patch \
    1014.rtl8139.patch \
    1016.gcc34x.patch \
    1017.ieee1394.patch \
    1018.kernel-preempt.patch \
    1019.corefile.patch \
    1020.rootfs-integration.patch \
    1021.lowlatency.patch \
    1022.ethernet.patch \
    1023.scard.patch \
    1024.usb.patch \
    1025.pcpartition.patch \
    1026.cpu-hb.patch \
    1027.RELEASE_NAME.patch \
    1028.reboot.patch \
    1029.depmod.patch \
    1030.ipfilter.patch \
    1031.kmalloc.patch \
    1032.gcc40x.patch \
    1033.cacheflush.patch \
    1034.isofs.patch \
    1036.sha1.patch
patch_executed_list_2_4_30 := $(patch_list_2_4_30:%=$(kernel_src_2_4_30_dir)/README.%)

$(patch_executed_list_2_4_30): $(kernel_src_2_4_30_dir)/.unpacked

$(kernel_src_2_4_30_dir)/.patched: $(patch_executed_list_2_4_30)
.PHONY: kernel-source-2.4.30-patched
kernel-source-2.4.30-patched: $(kernel_src_2_4_30_dir)/.patched

$(kernel_src_2_4_30_dir)/.configured: $(kernel_2_4_30_config) $(kernel_src_2_4_30_dir)/.patched
.PHONY: kernel-source-2.4.30-configured
kernel-source-2.4.30-configured: $(kernel_src_2_4_30_dir)/.configured

.PHONY: kernel-source-2.4.30
kernel-source-2.4.30: $(kernel_src_2_4_30_dir)/.configured

###############################################################################
#
# Kernel 2.6.15
#
###############################################################################

kernel_src_2_6_15           := linux-2.6.15.tar.bz2
kernel_src_2_6_15_dir       := linux-2.6.15
kernel_src_2_6_15_config    := config-2.6.15-smp86xx

$(kernel_src_2_6_15_dir)/.unpacked: $(kernel_src_2_6_15)
.PHONY: kernel-source-2.6.15-unpacked
kernel-source-2.6.15-unpacked: $(kernel_src_2_6_15_dir)/.unpacked

patch_list_2_6_15 :=  \
    0000.shared.patch \
    1000.tangox.patch \
    1001.pci.patch \
    1002.gpio.patch \
    1003.mbus.patch \
    1004.ide.patch \
    1005.tango2enet.patch \
    1006.usb.patch \
    1007.mtd.patch \
    1008.ir.patch \
    1009.fip.patch \
    1010.scard.patch \
    1011.udf.patch \
    1012.fb.patch \
    1013.frequency.patch \
    1014.ipfilter.patch \
    1015.cpucache.patch \
    1016.kmalloc.patch \
    1017.satasil.patch \
    1019.zbf.patch \
    1020.binary-identity.patch \
    1021.rootfs-integration.patch \
    1022.cacheflush.patch \
    1024.bug.patch \
    1025.ddma.patch \
    1027.sha.patch
patch_executed_list_2_6_15 := $(patch_list_2_6_15:%=$(kernel_src_2_6_15_dir)/README.%)

$(patch_executed_list_2_6_15): $(kernel_src_2_6_15_dir)/.unpacked

$(kernel_src_2_6_15_dir)/.patched: $(patch_executed_list_2_6_15)
.PHONY: kernel-source-2.6.15-patched
kernel-source-2.6.15-patched: $(kernel_src_2_6_15_dir)/.patched

$(kernel_src_2_6_15_dir)/.configured: $(kernel_src_2_6_15_config) $(kernel_src_2_6_15_dir)/.patched
.PHONY: kernel-source-2.6.15-configured
kernel-source-2.6.15-configured: $(kernel_src_2_6_15_dir)/.configured

.PHONY: kernel-source-2.6.15
kernel-source-2.6.15: $(kernel_src_2_6_15_dir)/.configured

###############################################################################
#
# Rules
#
###############################################################################
$(kernel_src_2_4_30) \
$(kernel_src_2_6_15):
	$(wget) $(download_site)/$(@F)

$(kernel_src_2_4_30_dir)/.unpacked \
$(kernel_src_2_6_15_dir)/.unpacked:
	bzcat $< | tar -x -f -
	touch $@

$(patch_executed_list_2_4_30) \
$(patch_executed_list_2_6_15): 
	patch -d $(@D) -p1 < $(patch_dir)/$(@D)/$(subst README.,,$(@F))

$(kernel_src_2_4_30_dir)/.patched \
$(kernel_src_2_6_15_dir)/.patched:
	touch $@

$(kernel_src_2_4_30_dir)/.configured \
$(kernel_src_2_6_15_dir)/.configured:
	sed -e "s/@CURRENT_UID@/$(shell echo $$UID)/;s/@CURRENT_GID@/$(shell echo $$GROUPS)/" < $< > $(@D)/.config
	make -C $(@D) oldconfig
	touch $@

kernel-source-2.4.30 \
kernel-source-2.6.15:
	@echo
	@echo "Your $(subst kernel-source-,,$@) kernel source is now available under ./$(<D)."
	@echo
