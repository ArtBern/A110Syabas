#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1995, 1998, 2001, 2002 by Ralf Baechle
# Copyright (C) 2004  Maciej W. Rozycki
#

#
# Some DECstations need all possible sections of an ECOFF executable
#
ifdef CONFIG_MACH_DECSTATION
  E2EFLAGS = -a
else
  E2EFLAGS =
endif

#
# Drop some uninteresting sections in the kernel.
# This is only relevant for ELF kernels but doesn't hurt a.out
#
drop-sections	= .reginfo .mdebug .comment .note .pdr .options .MIPS.options
strip-flags	= $(addprefix --remove-section=,$(drop-sections))

VMLINUX = vmlinux

all: vmlinux.ecoff vmlinux.srec addinitrd

vmlinux.ecoff: $(obj)/elf2ecoff $(VMLINUX)
	$(obj)/elf2ecoff $(VMLINUX) vmlinux.ecoff $(E2EFLAGS)

$(obj)/elf2ecoff: $(obj)/elf2ecoff.c
	$(HOSTCC) -o $@ $^

vmlinux.bin: $(VMLINUX)
	$(OBJCOPY) -O binary $(strip-flags) $(VMLINUX) $(obj)/vmlinux.bin

vmlinux.srec: $(VMLINUX)
	$(OBJCOPY) -S -O srec $(strip-flags) $(VMLINUX) $(obj)/vmlinux.srec

$(obj)/addinitrd: $(obj)/addinitrd.c
	$(HOSTCC) -o $@ $^

$(obj)/vmlinux.gz: $(obj)/vmlinux.bin
	gzip -c9 $< > $@

ifeq ($(CONFIG_TANGOX),y)
$(obj)/vmlinux.zbf: $(obj)/vmlinux.gz 
	genzbf -l $(loadaddr) -s $(loadaddr) -a lzf -o $@ $<

$(obj)/xrpc_xload_vmlinux_$(certtype).bin: $(obj)/vmlinux.zbf
	sh $(TOPDIR)/scripts/build_cpu_xload.bash $(obj)/vmlinux $(certid) $(certtype)

zbimage-linux-xrpc: $(obj)/zbimage-linux-xrpc

$(obj)/zbimage-linux-xrpc: $(obj)/xrpc_xload_vmlinux_$(certtype).bin
	mkdir romfs
	cp -f $< romfs
	genromfs -V MIPS_LINUX_XRPC -d romfs -f $@
	rm -rf romfs
endif

archhelp:
	@echo	'* vmlinux.ecoff	- ECOFF boot image'
	@echo	'* vmlinux.srec		- SREC boot image'

clean-files += addinitrd \
	       elf2ecoff \
	       vmlinux.bin \
	       vmlinux.ecoff \
	       vmlinux.srec
