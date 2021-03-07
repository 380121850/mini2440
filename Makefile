##########################################################################################
#	prepare param
##########################################################################################
export OSDRV_DIR=$(shell pwd)
export OSDRV_CROSS=arm-linux
export CHIP?=s3c2440
export ARCH=arm
export CROSS_COMPILE=$(OSDRV_CROSS)-
export OSDRV_CROSS_CFLAGS
MP_TYPE=sigle
BOOT_MEDIA?=spi

ifneq ($(BOOT_MEDIA),spi)
ifneq ($(BOOT_MEDIA),emmc)
$(error you must set valid BOOT_MEDIA:spi or emmc!)
endif
endif

ifneq ($(CHIP),s3c2440)
ifneq ($(CHIP),hi3516dv300)
ifneq ($(CHIP),hi3516av300)
$(error you must set valid CHIP:hi3516dv300 、 hi3516av300 or s3c2440!)
endif
endif
endif
ifeq ($(OSDRV_CROSS), )
$(error you must set OSDRV_CROSS first!)
endif
ifeq ($(OSDRV_CROSS),arm-himix100-linux)
LIB_TYPE:=uclibc
BUSYBOX_CFG:=config_v100_a7_softfp_neon
TOOLCHAIN_DIR:=arm-himix100-linux
TOOLCHAINI_VERSION:=
RUNTIME_LIB:=runtime_uclibc
CROSS_SPECIFIED:=y
endif

ifeq ($(OSDRV_CROSS),arm-linux)
LIB_TYPE:=glibc
RUNTIME_LIB:=runtime_glibc
CROSS_SPECIFIED:=y
endif

BUSYBOX_CFG:=fa.config
BUSYBOX_VER:=busybox-1.13.3
TOOLCHAIN_RUNTIME_LIB_C:=lib.tgz

TOOLCHAIN_RUNTIME_LIB:=a7_softfp_neon-vfpv4
OSDRV_CROSS_CFLAGS:=-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -w

UBOOT_VER:=bootloader/u-boot
UBOOT:=u-boot.bin
export UBOOT_CONFIG:=mini2440_config


KERNEL_VER:=linux-2.6.32.2
UIMAGE:=zImage
KERNEL_CFG:=config_mini2440_x35

PUB_IMAGE:=$(CHIP)_$(BOOT_MEDIA)_image_$(LIB_TYPE)

ROOT_FS_TAR:=rootfs_basic.tar.gz
PUB_ROOTFS:=rootfs_basic

EXT4_TOOL:=make_ext4fs
EXT4_IMAGE_BIN:=rootfs_$(CHIP)_96M.ext4
export PUB_BOARD:=board_$(LIB_TYPE)

JFFS2_IMAGE_BIN_64K:=rootfs_$(CHIP)_64k.jffs2
JFFS2_IMAGE_BIN_128K:=rootfs_$(CHIP)_128k.jffs2
JFFS2_IMAGE_BIN_256K:=rootfs_$(CHIP)_256k.jffs2

YAFFS2_PATCH:= hi_yaffs2utils.patch
YAFFS2_VER:= yaffs2utils-0.2.9
YAFFS2_TAR:=$(YAFFS2_VER).tar.gz
YAFFS_TOOL:=mkyaffs2image100
YAFFS2_IMAGE_BIN_2K_4BIT:=rootfs_$(CHIP)_2k_4bit.yaffs2
YAFFS2_IMAGE_BIN_2K_24BIT:=rootfs_$(CHIP)_2k_24bit.yaffs2
YAFFS2_IMAGE_BIN_4K_4BIT:=rootfs_$(CHIP)_4k_4bit.yaffs2
YAFFS2_IMAGE_BIN_4K_24BIT:=rootfs_$(CHIP)_4k_24bit.yaffs2

GDB_PATCH:= hi-gdb-7.9.1.patch
GDB_VER:= gdb-7.9.1
GDB_TAR:=$(GDB_VER).tar.gz
NCURSES_VER:= ncurses-6.0
NCURSES_TAR:=$(NCURSES_VER).tar.gz
CRAMFS_VER:= util-linux-2.31
CRAMFS_TAR:=$(CRAMFS_VER).tar.gz

HIREGBING_PACKAGE_VER:=hiregbin-v5.0.1
ifeq ($(TARGET_XLSM),)
ifeq ($(CHIP), hi3516dv300)
TARGET_XLSM:=Hi3516DV300-DMEB_4L_FLYBY-DDR3_1800M_512MB_16bitx2-A7_900M-SYSBUS_300M.xlsm
endif
ifeq ($(CHIP), hi3516av300)
TARGET_XLSM:=Hi3516AV300-DMEB_4L_FLYBY-DDR3_2133M_512MB_16bitx2-A7_900M-SYSBUS_300M.xlsm
endif
ifeq ($(CHIP), hi3516cv500)
TARGET_XLSM:=Hi3516CV500-DMEB_4L-DDR3_1800M_256MB_16bit-A7_900M-SYSBUS_300M.xlsm
endif
endif

TOOLCHAIN_FILE:= $(shell which $(OSDRV_CROSS)-gcc )
TOOLCHAIN_DIR:=$(shell dirname $(shell dirname $(TOOLCHAIN_FILE)))
RUNTIMELIB_DIR=$(shell dirname $(TOOLCHAIN_DIR))/$(OSDRV_CROSS)/$(RUNTIME_LIB)
##########################################################################################
#	set task
##########################################################################################
ifeq ($(CROSS_SPECIFIED),y)
ifeq ($(MP_TYPE),amp)
all: prepare hiboot hikernel hiliteos hirootfs_prepare hiliteos_sample hibusybox \
	hipctools hiboardtools hiipcm hirootfs_build
clean: hiboot_clean hikernel_clean \
       hibusybox_clean hipctools_clean hiboardtools_clean hirootfs_clean hiipcm_clean hiliteos_dirclean
else
all: prepare hiboot hikernel hirootfs_prepare hibusybox \
	hipctools hiboardtools hirootfs_build
clean: hiboot_clean hikernel_clean \
       hibusybox_clean hipctools_clean hiboardtools_clean hirootfs_clean
endif
notools: hiboot hikernel hinotools_prepare hirootfs_notools_build
distclean:clean pub_clean
endif

a:=$(shell $(OSDRV_CROSS)-gcc --version)
b:=$(findstring $(TOOLCHAINI_VERSION),$(a))
c:= $(word 2, $(a))
##########################################################################################
#task [0]	prepare
##########################################################################################
prepare:
	mkdir $(OSDRV_DIR)/pub/$(PUB_IMAGE) -p
	mkdir $(OSDRV_DIR)/pub/bin/$(PUB_BOARD) -p
	mkdir $(OSDRV_DIR)/pub/bin/pc -p

##########################################################################################
hinotools_prepare:
	pushd $(OSDRV_DIR)/pub;tar xzf $(PUB_ROOTFS).tgz;popd
##########################################################################################
#task [1]	build uboot
##########################################################################################
u-boot: prepare 
	@echo "---------task [1]	build boot"
	make -C $(OSDRV_DIR)/$(UBOOT_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- $(UBOOT_CONFIG)
	#find $(OSDRV_DIR)/$(UBOOT_VER) | xargs touch
	pushd $(OSDRV_DIR)/$(UBOOT_VER);make ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- -j 16 >/dev/null;popd
	cp $(OSDRV_DIR)/$(UBOOT_VER)/u-boot.bin $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(UBOOT)

u-boot_clean:
	make -C $(OSDRV_DIR)/$(UBOOT_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- distclean
	rm -rf $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(UBOOT)

##########################################################################################
#task [2]	build kernel
##########################################################################################
kernel: prepare 
	@echo "---------task [2] build kernel"
	cp $(OSDRV_DIR)/$(KERNEL_VER)/$(KERNEL_CFG) $(OSDRV_DIR)/$(KERNEL_VER)/arch/arm/configs/$(KERNEL_CFG)_defconfig
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- $(KERNEL_CFG)_defconfig
	pushd $(OSDRV_DIR)/$(KERNEL_VER);\
	make ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- uImage -j 16 >/dev/null;popd
	cp $(OSDRV_DIR)/$(KERNEL_VER)/arch/arm/boot/uImage $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(UIMAGE)

kernel_clean:
	rm $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(UIMAGE) -rf
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- distclean
	rm $(OSDRV_DIR)/$(KERNEL_VER)/arch/arm/configs/$(KERNEL_CFG)_defconfig -rf

##########################################################################################
#task [3]	prepare rootfs
##########################################################################################
rootfs_prepare: prepare
	@echo "---------task [3] prepare rootfs "
	tar xzf $(OSDRV_DIR)/rootfs/$(ROOT_FS_TAR) -C $(OSDRV_DIR)/pub

##########################################################################################
#task [4]	build busybox
##########################################################################################
busybox: prepare
	@echo "---------task [4] build busybox "
	cp $(OSDRV_DIR)/$(BUSYBOX_VER)/$(BUSYBOX_CFG) $(OSDRV_DIR)/$(BUSYBOX_VER)/.config
	pushd $(OSDRV_DIR)/$(BUSYBOX_VER)/;make -j 8 >/dev/null;popd
	make -C $(OSDRV_DIR)/$(BUSYBOX_VER) install
	cp -af $(OSDRV_DIR)/$(BUSYBOX_VER)/_install/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)

busybox_clean:
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/_install -rf
	make -C $(OSDRV_DIR)/$(BUSYBOX_VER) distclean

##########################################################################################
#task [5]	build pc tools
##########################################################################################
hipctools: prepare
	@echo "---------task [5] build tools which run on pc"
	tar xf $(PACKAGE_YAFFS2) -C $(OSDRV_DIR)/tools/pc/mkyaffs2image/
	pushd $(OSDRV_DIR)/tools/pc/mkyaffs2image/$(YAFFS2_VER);patch -p1 < ../$(YAFFS2_PATCH);popd
	find $(OSDRV_DIR)/tools/pc/mkyaffs2image/$(YAFFS2_VER) | xargs touch
	make -C $(OSDRV_DIR)/tools/pc/mkyaffs2image/
	cp $(OSDRV_DIR)/tools/pc/mkyaffs2image/bin/$(YAFFS_TOOL) $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/jffs2_tool/
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/mkfs.jffs2 $(OSDRV_DIR)/pub/bin/pc
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/mkfs.ubifs $(OSDRV_DIR)/pub/bin/pc
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/ubinize $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/cramfs_tool/
	cp $(OSDRV_DIR)/tools/pc/cramfs_tool/mkfs.cramfs $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/squashfs4.3/
	cp $(OSDRV_DIR)/tools/pc/squashfs4.3/mksquashfs $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/lzma_tool/
	cp $(OSDRV_DIR)/tools/pc/lzma_tool/lzma $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/ext4_utils/
	cp $(OSDRV_DIR)/tools/pc/ext4_utils/bin/$(EXT4_TOOL) $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/nand_production/fmc_nand_ecc_product_v100
	cp $(OSDRV_DIR)/tools/pc/nand_production/fmc_nand_ecc_product_v100/nand_product $(OSDRV_DIR)/pub/bin/pc

hipctools_clean:
	make -C $(OSDRV_DIR)/tools/pc/mkyaffs2image clean
	make -C $(OSDRV_DIR)/tools/pc/jffs2_tool distclean
	make -C $(OSDRV_DIR)/tools/pc/cramfs_tool distclean
	make -C $(OSDRV_DIR)/tools/pc/squashfs4.3 distclean
	make -C $(OSDRV_DIR)/tools/pc/lzma_tool distclean
	make -C $(OSDRV_DIR)/tools/pc/ext4_utils clean
	rm $(OSDRV_DIR)/tools/pc/uboot_tools/$(HIREGBING_PACKAGE_VER) -rf
	make -C $(OSDRV_DIR)/tools/pc/nand_production/fmc_nand_ecc_product_v100 clean

##########################################################################################
#task [6]	build board tools
##########################################################################################
hiboardtools: hirootfs_prepare hiboardtools_clean
	@echo "---------task [6] build tools which run on board "
	make -C $(OSDRV_DIR)/tools/board/e2fsprogs
	cp -af $(OSDRV_DIR)/tools/board/e2fsprogs/bin/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
	make -C $(OSDRV_DIR)/tools/board/reg-tools-1.0.0
	cp -af $(OSDRV_DIR)/tools/board/reg-tools-1.0.0/bin/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
	make -C $(OSDRV_DIR)/tools/board/eudev-3.2.7/
	cp -af $(OSDRV_DIR)/tools/board/eudev-3.2.7/rootfs/. $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/
	tar xf $(PACKAGE_GDB) -C $(OSDRV_DIR)/tools/board/gdb/
	pushd $(OSDRV_DIR)/tools/board/gdb/$(GDB_VER);patch -p1 < ../$(GDB_PATCH);popd
	find $(OSDRV_DIR)/tools/board/gdb/$(GDB_VER) | xargs touch
	make -C $(OSDRV_DIR)/tools/board/gdb
	cp $(OSDRV_DIR)/tools/board/gdb/gdb-$(OSDRV_CROSS) $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
	make -C $(OSDRV_DIR)/tools/board/mtd-utils/
	cp $(OSDRV_DIR)/tools/board/mtd-utils/bin/* $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
	cp $(OSDRV_DIR)/tools/board/reg-tools-1.0.0/bin/himm $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
	cp $(OSDRV_DIR)/tools/board/reg-tools-1.0.0/bin/himc $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
	cp $(OSDRV_DIR)/tools/board/reg-tools-1.0.0/bin/himd $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
	cp $(OSDRV_DIR)/tools/board/reg-tools-1.0.0/bin/himd.l $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)

hiboardtools_clean:
	make -C $(OSDRV_DIR)/tools/board/e2fsprogs distclean
	make -C $(OSDRV_DIR)/tools/board/reg-tools-1.0.0 clean
	make -C $(OSDRV_DIR)/tools/board/eudev-3.2.7 clean
	make -C $(OSDRV_DIR)/tools/board/gdb distclean
	make -C $(OSDRV_DIR)/tools/board/mtd-utils distclean

##########################################################################################
#task [7]	build rootfs
##########################################################################################
hirootfs_build: rootfs_prepare busybox hirootfs_notools_build
hirootfs_notools_build:
	@echo "---------task [7] build rootfs"
	chmod 777 $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/*
	chmod 777 $(OSDRV_DIR)/pub/bin/pc/*
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/* -rf
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin/himm -rf
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin/himc -rf
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin/himd -rf
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin/himd.l -rf
	pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS);ln -s sbin/init init;popd
	pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS); chmod 750 * -R; chmod 750 usr/* -R; popd
	pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS); chmod -w usr/bin -R; chmod -w usr/sbin -R; chmod -w sbin -R; popd

ifeq ($(BOOT_MEDIA),spi)
	pushd $(OSDRV_DIR)/pub/bin/pc;./mkfs.jffs2 -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -l -e 0x40000 -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(JFFS2_IMAGE_BIN_256K);popd
	pushd $(OSDRV_DIR)/pub/bin/pc;./mkfs.jffs2 -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -l -e 0x20000 -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(JFFS2_IMAGE_BIN_128K);popd
	pushd $(OSDRV_DIR)/pub/bin/pc;./mkfs.jffs2 -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -l -e 0x10000 -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(JFFS2_IMAGE_BIN_64K);popd

	cp $(OSDRV_DIR)/tools/pc/ubi_sh/mkubiimg.sh $(OSDRV_DIR)/pub/$(PUB_IMAGE)
	chmod +x $(OSDRV_DIR)/pub/$(PUB_IMAGE)/mkubiimg.sh
	# build the pagesize = 2k, blocksize = 128k, part_size = 32M #
	pushd $(OSDRV_DIR)/pub/$(PUB_IMAGE);./mkubiimg.sh $(CHIP) 2k 128k $(OSDRV_DIR)/pub/$(PUB_ROOTFS) 32M $(OSDRV_DIR)/pub/bin/pc;popd
	# build the pagesize = 4k, blocksize = 256k, part_size = 50M #
	pushd $(OSDRV_DIR)/pub/$(PUB_IMAGE);./mkubiimg.sh $(CHIP) 4k 256k $(OSDRV_DIR)/pub/$(PUB_ROOTFS) 50M $(OSDRV_DIR)/pub/bin/pc;popd
	rm $(OSDRV_DIR)/pub/$(PUB_IMAGE)/mkubiimg.sh
endif
ifeq ($(BOOT_MEDIA),emmc)
	pushd $(OSDRV_DIR)/pub/bin/pc;./$(EXT4_TOOL) -l 96M -s $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(EXT4_IMAGE_BIN) $(OSDRV_DIR)/pub/$(PUB_ROOTFS);popd
endif
ifeq ($(BOOT_MEDIA),spi)
	pushd $(OSDRV_DIR)/pub/bin/pc;./$(YAFFS_TOOL) $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN_2K_4BIT) 1 2;popd
	chmod 644 $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN_2K_4BIT)
	pushd $(OSDRV_DIR)/pub/bin/pc;./$(YAFFS_TOOL) $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN_2K_24BIT) 1 4;popd
	chmod 644 $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN_2K_24BIT)
	pushd $(OSDRV_DIR)/pub/bin/pc;./$(YAFFS_TOOL) $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN_4K_4BIT) 2 2;popd
	chmod 644 $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN_4K_4BIT)
	pushd $(OSDRV_DIR)/pub/bin/pc;./$(YAFFS_TOOL) $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN_4K_24BIT) 2 4;popd
	chmod 644 $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN_4K_24BIT)
endif
	find $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ -name '*svn' | xargs rm -rf
	pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS); chmod +w usr/bin -R; chmod +w usr/sbin -R; chmod +w sbin -R; popd
	pushd $(OSDRV_DIR)/pub;tar czf $(PUB_ROOTFS).tgz $(PUB_ROOTFS);rm $(PUB_ROOTFS) -rf;popd
	@echo "---------finish osdrv work"

rootfs_clean:
ifeq ($(OSDRV_DIR)/pub/$(PUB_ROOTFS), $(wildcard $(OSDRV_DIR)/pub/$(PUB_ROOTFS)))
	pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS); chmod +w usr/bin -R; chmod +w usr/sbin -R; chmod +w sbin -R; popd
endif
	rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ -rf

##########################################################################################
#task [9] ipcm build
##########################################################################################
# smp not support hiipcm
hiipcm:
	@echo -e $(INFO)"------- task [9] build ipcm"$(DONE)
	rm -rf  $(OSDRV_DIR)/components/ipcm/ipcm
	pushd $(OSDRV_DIR)/components/ipcm;tar xzf ipcm.tgz -C .;popd
	pushd  $(OSDRV_DIR)/components/ipcm/ipcm; make PLATFORM=$(CHIP) CFG=$(CHIP)_a7_liteos_config all; popd
	pushd  $(OSDRV_DIR)/components/ipcm/ipcm; make PLATFORM=$(CHIP) CFG=$(CHIP)_a7_linux_config all; popd
	cp $(OSDRV_DIR)/components/ipcm/ipcm/out/node_1/*.a $(OSDRV_DIR)/platform/liteos/out/$(CHIP)/lib
	cp $(OSDRV_DIR)/components/ipcm/ipcm/out/node_0/sharefs $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
	cp $(OSDRV_DIR)/components/ipcm/ipcm/out/node_0/virt-tty $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
	cp $(OSDRV_DIR)/components/ipcm/ipcm/out/node_0/*.ko $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/komod
hiipcm_clean:
	rm -rf $(OSDRV_DIR)/components/ipcm/ipcm
##########################################################################################
#task [10]	clean pub
##########################################################################################
pub_clean:
	rm $(OSDRV_DIR)/pub/* -rf

##########################################################################################
#task [11]  a7 liteos build
##########################################################################################
# smp not support hiliteos
hiliteos: prepare
ifneq ($(OSDRV_DIR)/platform/liteos, $(wildcard $(OSDRV_DIR)/platform/liteos))
	pushd $(OSDRV_DIR)/platform;tar xzf liteos.tgz;popd
else
	@echo -e $(NOTE)"Attention:"$(DONE)
	@echo -e $(NOTE)"you should compile ipcm for new libs!"$(DONE)
endif
	pushd $(OSDRV_DIR)/platform/liteos;cp ./tools/build/config/$(CHIP)_defconfig .config;make -j 20 >/dev/null;popd

hiliteos_clean:
	make -C $(OSDRV_DIR)/platform/liteos clean
hiliteos_distclean:
	make -C $(OSDRV_DIR)/platform/liteos clean
hiliteos_dirclean:
	rm -rf $(OSDRV_DIR)/platform/liteos

##########################################################################################
#task [12]  a7 liteos sample.bin build
##########################################################################################
# smp not support hiliteos
hiliteos_sample: prepare hiliteos
	pushd $(OSDRV_DIR)/platform/liteos/sample/sample_osdrv; \
	make >/dev/null; \
	cp sample.bin $(OSDRV_DIR)/pub/$(PUB_IMAGE); \
	popd

hiliteos_sample_clean:
	make -C $(OSDRV_DIR)/platform/liteos/sample/sample_osdrv clean
