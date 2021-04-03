##########################################################################################
#	prepare param
##########################################################################################
export OSDRV_DIR=$(shell pwd)
export OSDRV_CROSS=arm-linux
export CHIP?=s3c2440
export ARCH=arm
export CROSS_COMPILE=$(OSDRV_CROSS)-
CROSS_COMPILE_STRIP:=$(OSDRV_CROSS)-strip
export OSDRV_CROSS_CFLAGS
MP_TYPE=sigle
BOOT_MEDIA?=spi
PUB_BOARD:=board

PUB_BIN_BOARD_DIR=$(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
PUB_BIN_PC_DIR=$(OSDRV_DIR)/pub/bin/pc
PUB_IMAGE_DIR=$(OSDRV_DIR)/pub/$(CHIP)_$(BOOT_MEDIA)_image_$(LIB_TYPE)


ifneq ($(BOOT_MEDIA),spi)
$(error you must set valid BOOT_MEDIA:spi!)
endif

ifneq ($(CHIP),s3c2440)
$(error you must set valid CHIP:s3c2440!)
endif

ifeq ($(OSDRV_CROSS), )
$(error you must set OSDRV_CROSS first!)
endif

ifeq ($(OSDRV_CROSS),arm-linux)
LIB_TYPE:=glibc
RUNTIME_LIB:=runtime_glibc
CROSS_SPECIFIED:=y
endif

BUSYBOX_CFG:=fa.config
BUSYBOX_VER:=busybox-1.32.1
TOOLCHAIN_RUNTIME_LIB_C:=lib.tgz

TOOLCHAIN_RUNTIME_LIB:=a7_softfp_neon-vfpv4
OSDRV_CROSS_CFLAGS:=-mcpu=arm920t -mfloat-abi=softfp -w
#  -mfpu=neon-vfpv4

UBOOT_VER:=bootloader/u-boot
UBOOT:=u-boot.bin
UBOOT_CONFIG:=mini2440_config


KERNEL_VER:=linux-2.6.32.2
UIMAGE:=uImage
KERNEL_CFG:=config_mini2440_x35
KERNEL_DIR=$(OSDRV_DIR)/$(KERNEL_VER)
KO_TARGET_DIR=$(OSDRV_DIR)/pub/app/

PUB_KERNEL_TARGET_DIR=$(KERNEL_DIR)/build_$(CHIP)


ROOT_FS_TAR:=rootfs_basic.tar.gz
PUB_ROOTFS:=rootfs_basic

APP_FS_TAR:=app.tar.gz
PUB_APPFS:=app
PUB_APP_DIR:=$(OSDRV_DIR)/pub/$(PUB_APPFS)/

EXT4_TOOL:=make_ext4fs
EXT4_IMAGE_BIN:=rootfs_$(CHIP)_96M.ext4


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
ZLIB_TAR:=zlib-1.2.11.tar.gz
PCRE_TAR:=pcre-8.44.tar.gz
OPENSSL_TAR:=openssl-1.1.1j.tar.gz

ZLIB_DIR:=$(OSDRV_DIR)/tools/board/zlib/
PCRE_DIR:=$(OSDRV_DIR)/tools/board/pcre/
OPENSSL_DIR:=$(OSDRV_DIR)/tools/board/openssl/

ZLIB_UNPACK_DIR:=$(ZLIB_DIR)/zlib-1.2.11/
PCRE_UNPACK_DIR:=$(PCRE_DIR)/pcre-8.44/
OPENSSL_UNPACK_DIR:=$(OPENSSL_DIR)/openssl-1.1.1j/

NGINX_DIR:=$(OSDRV_DIR)/tools/board/nginx-1.18.0/
NGINX_TARGET_DIR:=$(PUB_BIN_BOARD_DIR)/nginx

TOOLCHAIN_FILE:= $(shell which $(OSDRV_CROSS)-gcc )
TOOLCHAIN_DIR:=$(shell dirname $(shell dirname $(TOOLCHAIN_FILE)))
RUNTIMELIB_DIR=$(shell dirname $(TOOLCHAIN_DIR))/$(OSDRV_CROSS)/$(RUNTIME_LIB)



##########################################################################################
#	set task
##########################################################################################
ifeq ($(CROSS_SPECIFIED),y)
all: prepare hiboot hikernel hirootfs_prepare hibusybox hipctools hiboardtools hirootfs_build

clean: u-boot_clean kernel_clean pctools_clean busybox_clean  pub_clean boardtools_clean
		rm -rf $(OSDRV_DIR)/pub/
endif

a:=$(shell $(OSDRV_CROSS)-gcc --version)
b:=$(findstring $(TOOLCHAINI_VERSION),$(a))
c:= $(word 2, $(a))
##########################################################################################
#task [0]	prepare
##########################################################################################
prepare:
	mkdir $(PUB_BIN_BOARD_DIR) -p
	mkdir $(PUB_BIN_PC_DIR) -p
	mkdir $(PUB_IMAGE_DIR) -p
	mkdir $(NGINX_TARGET_DIR) -p
	mkdir $(PUB_KERNEL_TARGET_DIR)  -p

##########################################################################################
#task [1]	build uboot
##########################################################################################
u-boot: prepare 
	@echo "---------task [1]	build boot"
	make -C $(OSDRV_DIR)/$(UBOOT_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- $(UBOOT_CONFIG)
	#find $(OSDRV_DIR)/$(UBOOT_VER) | xargs touch
	pushd $(OSDRV_DIR)/$(UBOOT_VER);make ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- -j 16 >/dev/null;popd
	cp $(OSDRV_DIR)/$(UBOOT_VER)/u-boot.bin $(PUB_IMAGE_DIR)/$(UBOOT)

u-boot_clean:
	make -C $(OSDRV_DIR)/$(UBOOT_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- distclean
	rm -rf $(PUB_IMAGE_DIR)/$(UBOOT)

##########################################################################################
#	check_package :kernel and yaffs2 source file
##########################################################################################
check_kernel:
ifeq ($(PACKAGE_YAFFS2), $(YAFFS2_TAR))
result=$(shell echo "pass" || "fail")
$(warning $(result))
endif
PACKAGE_YAFFS2 = $(shell find $(OSDRV_DIR)/tools/pc/mkyaffs2image/ \
					-name "$(YAFFS2_TAR)")

ifeq ($(PACKAGE_GDB), $(GDB_TAR))
	result=$(shell echo "pass" || "fail")
	$(warning $(result))
endif

ifeq ($(PACKAGE_NCURSES), $(NCURSES_TAR))
	result=$(shell echo "pass" || "fail")
	$(warning $(result))
endif

ifeq ($(PACKAGE_CRAMFS), $(CRAMFS_TAR))
	result=$(shell echo "pass" || "fail")
	$(warning $(result))
endif

PACKAGE_GDB = $(shell find $(OSDRV_DIR)/tools/board/gdb/ -name "$(GDB_TAR)")
PACKAGE_NCURSES = $(shell find $(OSDRV_DIR)/tools/board/gdb/ -name "$(NCURSES_TAR)")
PACKAGE_CRAMFS = $(shell find $(OSDRV_DIR)/tools/pc/cramfs_tool/ -name "$(CRAMFS_TAR)")

ifeq ($(PACKAGE_YAFFS2), )
$(warning "---------------------------------------------------------------------")
$(warning "     Cannot found the $(YAFFS2_TAR)  source file		                ")
$(warning "   Please download the compression package to osdrv/tools/pc/mkyaffs2image/")
$(warning "---------------------------------------------------------------------")
$(error )
endif

ifeq ($(PACKAGE_GDB), )
$(warning "---------------------------------------------------------------------")
$(warning "     Cannot found the $(GDB_TAR)  source file						")
$(warning "   Please download the compression package to osdrv/tools/board/gdb/ ")
$(warning "---------------------------------------------------------------------")
$(error )
endif

ifeq ($(PACKAGE_NCURSES), )
$(warning "---------------------------------------------------------------------")
$(warning "     Cannot found the $(NCURSES_TAR) source file		                ")
$(warning "   Please download the compression package to osdrv/tools/board/gdb/ ")
$(warning "---------------------------------------------------------------------")
$(error )
endif

ifeq ($(PACKAGE_CRAMFS), )
$(warning "---------------------------------------------------------------------")
$(warning "     Cannot found the $(CRAMFS_TAR) source file		                ")
$(warning "   Please download the compression package to osdrv/tools/pc/cramfs_tool/ ")
$(warning "---------------------------------------------------------------------")
$(error )
endif
##########################################################################################
#task [2]	build kernel
##########################################################################################
kernel: prepare
	@echo "---------task [2] build kernel"
	cp $(OSDRV_DIR)/$(KERNEL_VER)/$(KERNEL_CFG) $(OSDRV_DIR)/$(KERNEL_VER)/arch/arm/configs/$(KERNEL_CFG)_defconfig
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)-  $(KERNEL_CFG)_defconfig
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)-  uImage -j 16 
	cp $(PUB_KERNEL_TARGET_DIR)/arch/arm/boot/uImage $(PUB_IMAGE_DIR)/$(UIMAGE)

kernel_menuconfig:
	cp $(OSDRV_DIR)/$(KERNEL_VER)/$(KERNEL_CFG) $(OSDRV_DIR)/$(KERNEL_VER)/arch/arm/configs/$(KERNEL_CFG)_defconfig
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)-  $(KERNEL_CFG)_defconfig
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)-  menuconfig

kernel_savecfg:
	cp $(OSDRV_DIR)/$(KERNEL_VER)/.config $(OSDRV_DIR)/$(KERNEL_VER)/$(KERNEL_CFG);

kernel_modules: 
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- modules -j 16
	
kernel_clean:
	rm $(PUB_IMAGE_DIR)/$(UIMAGE) -rf
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- distclean
	rm $(OSDRV_DIR)/$(KERNEL_VER)/arch/arm/configs/$(KERNEL_CFG)_defconfig -rf
	rm $(PUB_KERNEL_TARGET_DIR) -rf
	
##########################################################################################
#task [3]	prepare rootfs
##########################################################################################
rootfs_prepare: prepare busybox
	@echo "---------task [3] prepare rootfs "
	tar xzf $(OSDRV_DIR)/rootfs/$(ROOT_FS_TAR) -C $(OSDRV_DIR)/pub
	cp -af $(OSDRV_DIR)/$(BUSYBOX_VER)/_install/* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)

rootfs_dev: prepare
	@echo "---------task [4] prepare rootfs "
	mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/console c 5 1

##########################################################################################
#task [4]	build busybox
##########################################################################################
busybox: 
	@echo "---------task [4] build busybox "
	cp $(OSDRV_DIR)/$(BUSYBOX_VER)/$(BUSYBOX_CFG) $(OSDRV_DIR)/$(BUSYBOX_VER)/.config
	pushd $(OSDRV_DIR)/$(BUSYBOX_VER)/;make -j 16 >/dev/null;popd
	make -C $(OSDRV_DIR)/$(BUSYBOX_VER) install

busybox_clean:
	make -C $(OSDRV_DIR)/$(BUSYBOX_VER) distclean
	rm $(OSDRV_DIR)/$(BUSYBOX_VER)/_install/ -rf

##########################################################################################
#task [5]	build pc tools
##########################################################################################
pctools: pctools_clean prepare
	@echo "---------task [5] build tools which run on pc"
	#tar xf $(PACKAGE_YAFFS2) -C $(OSDRV_DIR)/tools/pc/mkyaffs2image/
	#pushd $(OSDRV_DIR)/tools/pc/mkyaffs2image/$(YAFFS2_VER);patch -p1 < ../$(YAFFS2_PATCH);popd
	#find $(OSDRV_DIR)/tools/pc/mkyaffs2image/$(YAFFS2_VER) | xargs touch
	#make -C $(OSDRV_DIR)/tools/pc/mkyaffs2image/ -j 16
	#cp $(OSDRV_DIR)/tools/pc/mkyaffs2image/bin/$(YAFFS_TOOL) $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/jffs2_tool/
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/mkfs.jffs2 $(OSDRV_DIR)/pub/bin/pc
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/mkfs.ubifs $(OSDRV_DIR)/pub/bin/pc
	cp $(OSDRV_DIR)/tools/pc/jffs2_tool/ubinize $(OSDRV_DIR)/pub/bin/pc
	#make -C $(OSDRV_DIR)/tools/pc/cramfs_tool/ -j 16
	#cp $(OSDRV_DIR)/tools/pc/cramfs_tool/mkfs.cramfs $(OSDRV_DIR)/pub/bin/pc
	#make -C $(OSDRV_DIR)/tools/pc/squashfs4.3/ -j 16
	#cp $(OSDRV_DIR)/tools/pc/squashfs4.3/mksquashfs $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/lzma_tool/ -j 16
	cp $(OSDRV_DIR)/tools/pc/lzma_tool/lzma $(OSDRV_DIR)/pub/bin/pc
	make -C $(OSDRV_DIR)/tools/pc/ext4_utils/ -j 16
	cp $(OSDRV_DIR)/tools/pc/ext4_utils/bin/$(EXT4_TOOL) $(OSDRV_DIR)/pub/bin/pc
	#make -C $(OSDRV_DIR)/tools/pc/nand_production/fmc_nand_ecc_product_v100
	#cp $(OSDRV_DIR)/tools/pc/nand_production/fmc_nand_ecc_product_v100/nand_product $(OSDRV_DIR)/pub/bin/pc

pctools_clean:
	#make -C $(OSDRV_DIR)/tools/pc/mkyaffs2image clean
	make -C $(OSDRV_DIR)/tools/pc/jffs2_tool distclean
	#make -C $(OSDRV_DIR)/tools/pc/cramfs_tool distclean
	#make -C $(OSDRV_DIR)/tools/pc/squashfs4.3 distclean
	make -C $(OSDRV_DIR)/tools/pc/lzma_tool distclean
	make -C $(OSDRV_DIR)/tools/pc/ext4_utils clean
	#rm $(OSDRV_DIR)/tools/pc/uboot_tools/$(HIREGBING_PACKAGE_VER) -rf
	#make -C $(OSDRV_DIR)/tools/pc/nand_production/fmc_nand_ecc_product_v100 clean

##########################################################################################
#task [6]	build board tools
##########################################################################################
boardtools:
	@echo "---------task [6] build tools which run on board "
	make -C $(OSDRV_DIR)/tools/board/e2fsprogs
	#tar xf $(PACKAGE_GDB) -C $(OSDRV_DIR)/tools/board/gdb/
	#pushd $(OSDRV_DIR)/tools/board/gdb/$(GDB_VER);patch -p1 < ../$(GDB_PATCH);popd
	#find $(OSDRV_DIR)/tools/board/gdb/$(GDB_VER) | xargs touch
	#make -C $(OSDRV_DIR)/tools/board/gdb
	#cp $(OSDRV_DIR)/tools/board/gdb/gdb-$(OSDRV_CROSS) $(OSDRV_DIR)/pub/$(PUB_APPFS)/sbin
	make -C $(OSDRV_DIR)/tools/board/mtd-utils/
	
	pushd $(ZLIB_DIR);tar zxf $(ZLIB_TAR) ;popd
	pushd $(PCRE_DIR);tar zxf $(PCRE_TAR) ;popd
	pushd $(OPENSSL_DIR);tar zxf $(OPENSSL_TAR) ;popd
	pushd $(NGINX_DIR);sh buildcfg.sh ;popd
	make -C $(NGINX_DIR) -j 16
	make -C $(NGINX_DIR) install
	
.PHONY:boardtools_clean
boardtools_clean:
	make -C $(OSDRV_DIR)/tools/board/e2fsprogs distclean
	make -C $(OSDRV_DIR)/tools/board/gdb distclean
	make -C $(OSDRV_DIR)/tools/board/mtd-utils distclean
	rm -rf  $(ZLIB_UNPACK_DIR)
	rm -rf  $(PCRE_UNPACK_DIR)
	rm -rf  $(OPENSSL_UNPACK_DIR)
	rm -rf  $(NGINX_TARGET_DIR)
	#make -C $(NGINX_DIR) clean
	
##########################################################################################
#task [7]	build rootfs
##########################################################################################
#hirootfs_build: rootfs_prepare busybox hirootfs_notools_build
app_prepare:prepare
	@echo "---------task [7] build rootfs"
	#chmod 777 -R $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/*
	#chmod 777 -R $(OSDRV_DIR)/pub/bin/pc/*
	
	tar xzf $(OSDRV_DIR)/rootfs/$(APP_FS_TAR) -C $(OSDRV_DIR)/pub

	cp $(OSDRV_DIR)/tools/pc/ubi_sh/mkubiimg.sh $(PUB_IMAGE_DIR)
	chmod +x $(PUB_IMAGE_DIR)/mkubiimg.sh

app:app_clean app_prepare pctools boardtools kernel_modules

	cp -af $(OSDRV_DIR)/tools/board/e2fsprogs/bin/* $(PUB_APP_DIR)/sbin
	cp -af $(OSDRV_DIR)/tools/board/mtd-utils/bin/* $(PUB_APP_DIR)/sbin
	cp -rf $(NGINX_TARGET_DIR) $(PUB_APP_DIR)/opt/
	rm $(PUB_APP_DIR)/sbin/flash_eraseall
	mv $(PUB_APP_DIR)/sbin/update-usbids.sh /tmp
	mv $(PUB_APP_DIR)/sbin/shutdown /tmp
	$(CROSS_COMPILE_STRIP) $(PUB_APP_DIR)/sbin/*
	cp -af $(OSDRV_DIR)/tools/board/mtd-utils/bin/flash_eraseall $(PUB_APP_DIR)/sbin
	cp -af /tmp/update-usbids.sh $(PUB_APP_DIR)/sbin
	cp -af /tmp/shutdown $(PUB_APP_DIR)/sbin
	$(CROSS_COMPILE_STRIP) $(PUB_APP_DIR)/opt/nginx/sbin/*
	
	make -C $(OSDRV_DIR)/$(KERNEL_VER) ARCH=arm CROSS_COMPILE=$(OSDRV_CROSS)- O=$(PUB_KERNEL_TARGET_DIR) modules_install INSTALL_MOD_PATH=$(KO_TARGET_DIR)
	#EXT4
	#cp $(KERNEL_DIR)/fs/mbcache.ko   $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/fs/jbd2/jbd2.ko $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/fs/ext4/ext4.ko $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/lib/crc16.ko    $(KO_TARGET_DIR)/
	
	#NFS
	#cp $(KERNEL_DIR)/fs/lockd/lockd.ko                         $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/crypto/cbc.ko                             $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/fs/nfs/nfs.ko                             $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/net/sunrpc/sunrpc.ko                      $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/crypto/md5.ko                             $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/net/sunrpc/auth_gss/rpcsec_gss_krb5.ko    $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/fs/nfs_common/nfs_acl.ko                  $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/crypto/des_generic.ko                     $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/net/sunrpc/auth_gss/auth_rpcgss.ko        $(KO_TARGET_DIR)/

	#mmc
	#cp $(KERNEL_DIR)/drivers/mmc/card/mmc_block.ko   $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/drivers/mmc/core/mmc_core.ko    $(KO_TARGET_DIR)/
	#cp $(KERNEL_DIR)/drivers/mmc/host/s3cmci.ko      $(KO_TARGET_DIR)/

	#$(OSDRV_CROSS)-strip $(OSDRV_DIR)/pub/$(PUB_APPFS)/sbin/* >/dev/null;
	#$(OSDRV_CROSS)-strip $(OSDRV_DIR)/pub/$(PUB_APPFS)/bin/* >/dev/null;
	#$(OSDRV_CROSS)-strip $(OSDRV_DIR)/pub/$(PUB_APPFS)/lib/* >/dev/null;
	# build the pagesize = 2k, blocksize = 128k, part_size = 254336KB #
	pushd $(PUB_IMAGE_DIR);./mkubiimg.sh  $(CHIP) 2k 128k $(OSDRV_DIR)/pub/$(PUB_APPFS) 254336KB $(OSDRV_DIR)/pub/bin/pc 0 app;popd
	
	rm $(PUB_IMAGE_DIR)/mkubiimg.sh

	@echo "---------finish osdrv work"

app_clean:pctools_clean boardtools_clean
	rm $(OSDRV_DIR)/pub/$(PUB_APPFS)/ -rf

##########################################################################################
#task [10]	clean pub
##########################################################################################
pub_clean:
	rm $(OSDRV_DIR)/pub/* -rf