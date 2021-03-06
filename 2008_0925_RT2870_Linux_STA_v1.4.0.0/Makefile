#MODE - STA or AP or APSTA.
RT28xx_MODE = STA

#TARTET - LINUX or UCOS.
TARGET = LINUX

#CHIPSET - 2860 or 2870.
CHIPSET = 2870

#RT28xx_DIR = home directory of RT28xx source code
RT28xx_DIR = $(shell pwd)
RTMP_SRC_DIR = $(RT28xx_DIR)/RT$(CHIPSET)
#PLATFORM = 5VT
#PLATFORM = PC
#PLATFORM = IKANOS_V160
#PLATFORM = IKANOS_V180
PLATFORM = SIGMA
#PLATFORM = SIGMA_8622
#PLATFORM = INIC
#PLATFORM = STAR
#PLATFORM = IXP
#PLATFORM = INF_TWINPASS
#PLATFORM = INF_DANUBE
#PLATFORM = BRCM_6358
#PLATFORM = INF_AMAZON_SE
#PLATFORM = CAVM_OCTEON
RELEASE = DPO

ifeq ($(PLATFORM),5VT)
LINUX_SRC = /opt/fvt_11N_SDK_0807/fvt131x_SDK_11n/linux-2.6.17
CROSS_COMPILE = /opt/crosstool/uClibc_v5te_le_gcc_4_1_1/bin/arm-linux-
endif

ifeq ($(PLATFORM),IKANOS_V160)
LINUX_SRC = /home/sample/projects/LX_2618_RG_5_3_00r4_SRC/linux-2.6.18
CROSS_COMPILE = mips-linux-
endif

ifeq ($(PLATFORM),IKANOS_V180)
LINUX_SRC = /home/sample/projects/LX_BSP_VX180_5_4_0r1_ALPHA_26DEC07/linux-2.6.18
CROSS_COMPILE = mips-linux-
endif

ifeq ($(PLATFORM),SIGMA)
LINUX_SRC = /opt/rt2870/A110Syabas/linux-2.6.15
CROSS_COMPILE = /opt/rt2870/smp86xx_toolchain.20080505/build_mipsel_nofpu/staging_dir/bin/mipsel-linux-
endif

ifeq ($(PLATFORM),SIGMA_8622)
LINUX_SRC = /home/snowpin/armutils_2.5.120.1/build_arm/linux-2.4.22-em86xx
CROSS_COMPILE = /home/snowpin/armutils_2.5.120.1/toolchain/bin/arm-elf-
CROSS_COMPILE_INCLUDE = /home/snowpin/armutils_2.5.120.1/toolchain/lib/gcc-lib/arm-elf/2.95.3
endif

ifeq ($(PLATFORM),INIC)
UCOS_SRC = /opt/uCOS/iNIC_rt2880
CROSS_COMPILE = /usr/bin/mipsel-linux-
endif

ifeq ($(PLATFORM),STAR)
LINUX_SRC = /opt/star/kernel/linux-2.4.27-star
CROSS_COMPILE = /opt/star/tools/arm-linux/bin/arm-linux-
endif

ifeq ($(PLATFORM),PC)
# Linux 2.6
LINUX_SRC = /lib/modules/$(shell uname -r)/build
# Linux 2.4 Change to your local setting
#LINUX_SRC = /usr/src/linux-2.4
LINUX_SRC_MODULE = /lib/modules/$(shell uname -r)/kernel/drivers/net/wireless/
CROSS_COMPILE = 
endif

ifeq ($(PLATFORM),IXP)
LINUX_SRC = /project/stable/Gmtek/snapgear-uclibc/linux-2.6.x
CROSS_COMPILE = arm-linux-
endif

ifeq ($(PLATFORM),INF_TWINPASS)
# Linux 2.6
#LINUX_SRC = /lib/modules/$(shell uname -r)/build
# Linux 2.4 Change to your local setting
LINUX_SRC = /project/stable/twinpass/release/2.0.1/source/kernel/opensource/linux-2.4.31/
CROSS_COMPILE = mips-linux-
endif

ifeq ($(PLATFORM),INF_DANUBE)
LINUX_SRC = /opt/danube/sdk/linux-2.6.16.x
CROSS_COMPILE = mips-linux-
ROOTDIR = /opt/danube/sdk
export ROOTDIR
endif

ifeq ($(PLATFORM),BRCM_6358)
LINUX_SRC = 
CROSS_COMPILE = 
endif

ifeq ($(PLATFORM),INF_AMAZON_SE)
# Linux 2.6
#LINUX_SRC = /lib/modules/$(shell uname -r)/build
# Linux 2.4 Change to your local setting
LINUX_SRC = /backup/ifx/3.6.2.2/source/kernel/opensource/linux-2.4.31
#CROSS_COMPILE = mips-linux-
#LINUX_SRC = /project/Infineon/3.6.2.2/source/kernel/opensource/linux-2.4.31
CROSS_COMPILE = /opt/uclibc-toolchain/ifx-lxdb-1-2-3-external/gcc-3.3.6/toolchain-mips/R0208V35/mips-linux-uclibc/bin/
endif

ifeq ($(PLATFORM),CAVM_OCTEON)
OCTEON_ROOT = /usr/local/Cavium_Networks/OCTEON-SDK
LINUX_SRC = $(OCTEON_ROOT)/linux/kernel_2.6/linux
CROSS_COMPILE = mips64-octeon-linux-gnu-
endif

export RT28xx_DIR RT28xx_MODE LINUX_SRC CROSS_COMPILE CROSS_COMPILE_INCLUDE PLATFORM RELEASE CHIPSET RTMP_SRC_DIR LINUX_SRC_MODULE

all: build_tools $(TARGET)


build_tools:
	make -C tools
	$(RT28xx_DIR)/tools/bin2h

UCOS:
	make -C os/ucos/ MODE=$(RT28xx_MODE)
	echo $(RT28xx_MODE)


LINUX:
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	cp -f os/linux/Makefile.4 $(RT28xx_DIR)/os/linux/Makefile
	make -C $(RT28xx_DIR)/os/linux/
ifeq ($(RT28xx_MODE),AP)
	cp -f $(RT28xx_DIR)/os/linux/rt$(CHIPSET)ap.o /tftpboot
else	
ifeq ($(RT28xx_MODE),APSTA)
	cp -f $(RT28xx_DIR)/os/linux/rt$(CHIPSET)apsta.o /tftpboot
else
	cp -f $(RT28xx_DIR)/os/linux/rt$(CHIPSET)sta.o /tftpboot
endif	
endif	
else
	cp -f os/linux/Makefile.6 $(RT28xx_DIR)/os/linux/Makefile
	make  -C  $(LINUX_SRC) SUBDIRS=$(RT28xx_DIR)/os/linux modules
ifeq ($(RT28xx_MODE),AP)
	cp -f $(RT28xx_DIR)/os/linux/rt$(CHIPSET)ap.ko /tftpboot
else	
ifeq ($(RT28xx_MODE),APSTA)
	cp -f $(RT28xx_DIR)/os/linux/rt$(CHIPSET)apsta.ko /tftpboot
else
	cp -f $(RT28xx_DIR)/os/linux/rt$(CHIPSET)sta.ko /tftpboot
endif	
endif
endif

release:
ifeq ($(TARGET), LINUX)
	make -C $(RT28xx_DIR)/os/linux -f Makefile.release release
ifeq ($(RELEASE), DPA)
	make -C $(RT28xx_DIR)/os/linux -f Makefile.DPA release
	rm -rf build
endif
ifeq ($(RELEASE), DPB)
	make -C $(RT28xx_DIR)/os/linux -f Makefile.DPB release
	rm -rf build
endif
ifeq ($(RELEASE), DPC)
	make -C $(RT28xx_DIR)/os/linux -f Makefile.DPC release
	rm -rf build
endif
ifeq ($(RT28xx_MODE),STA)
ifeq ($(RELEASE), DPD)
	make -C $(RT28xx_DIR)/os/linux -f Makefile.DPD release
	rm -rf build
endif
ifeq ($(RELEASE), DPO)
	make -C $(RT28xx_DIR)/os/linux -f Makefile.DPO release
	rm -rf build
endif
endif
endif

prerelease:
	make -C $(RT28xx_DIR)/os/linux -f Makefile.release prerelease
	cp $(RT28xx_DIR)/os/linux/Makefile.DPB $(RTMP_SRC_DIR)/os/linux/.
	cp $(RT28xx_DIR)/os/linux/Makefile.DPA $(RTMP_SRC_DIR)/os/linux/.
	cp $(RT28xx_DIR)/os/linux/Makefile.DPC $(RTMP_SRC_DIR)/os/linux/.
ifeq ($(RT28xx_MODE),STA)
	cp $(RT28xx_DIR)/os/linux/Makefile.DPD $(RTMP_SRC_DIR)/os/linux/.
endif	

clean:
ifeq ($(TARGET), LINUX)
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	cp -f os/linux/Makefile.4 os/linux/Makefile
else
	cp -f os/linux/Makefile.6 os/linux/Makefile
endif
	make -C os/linux clean
	rm -rf os/linux/Makefile
endif	
ifeq ($(TARGET), UCOS)
	make -C os/ucos clean MODE=$(RT28xx_MODE)
endif

uninstall:
ifeq ($(TARGET), LINUX)
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	make -C $(RT28xx_DIR)/os/linux -f Makefile.4 uninstall
else
	make -C $(RT28xx_DIR)/os/linux -f Makefile.6 uninstall
endif
endif

install:
ifeq ($(TARGET), LINUX)
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	make -C $(RT28xx_DIR)/os/linux -f Makefile.4 install
else
	make -C $(RT28xx_DIR)/os/linux -f Makefile.6 install
endif
endif
