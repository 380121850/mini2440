TOOL_BIN := bin
OSDRV_CROSS ?=arm-himix200-linux
OSDRV_CROSS_CFLAGS ?= -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4

TOOL_NAME :=mtd-utils-2.0.2
TOOL_TAR_BALL := mtd-utils-2.0.2.tar.bz2
TOOL_DIR := $(shell pwd)

ZLIB_NAME:=zlib-1.2.11
ZLIB_TAR_BALL:=zlib-1.2.11.tar.gz
ZLIB_LIB_PATH :=$(TOOL_DIR)/$(ZLIB_NAME)/zlib_install/lib/libz.a

UUID_NAME:=libuuid-1.0.3
UUID_TAR_BALL:=libuuid-1.0.3.tar.gz
LIBUUID_INSTALL :=$(TOOL_DIR)/$(UUID_NAME)/_install
LIBUUID_LIB_PATH :=$(LIBUUID_INSTALL)/lib/libuuid.a

INCLUDE += -I$(TOOL_DIR)/$(ZLIB_NAME)/zlib_install/include

all:	zlib libuuid
	rm $(TOOL_NAME) -rf;
	tar -xf $(TOOL_TAR_BALL);
	mkdir -p $(TOOL_DIR)/$(TOOL_NAME)/$(TOOL_BIN)/;
	mkdir -p $(TOOL_BIN)/;
	pushd $(TOOL_NAME)/;\
		./configure --without-lzo --host=$(OSDRV_CROSS) CFLAGS="$(OSDRV_CROSS_CFLAGS)" \
		LDFLAGS="$(OSDRV_CROSS_CFLAGS)" --prefix=$(TOOL_DIR)/$(TOOL_NAME)/$(TOOL_BIN) \
		ZLIB_LIBS=$(ZLIB_LIB_PATH) UUID_LIBS=$(LIBUUID_LIB_PATH) --disable-tests; \
		make CFLAGS=--static INCLUDES=$(INCLUDE) -j 20 > /dev/null; \
		make install > /dev/null;\
	popd
	cp -rf $(TOOL_DIR)/$(TOOL_NAME)/$(TOOL_BIN)/sbin/* $(TOOL_BIN)/

zlib:
	rm $(ZLIB_NAME) -rf;
	tar -xf $(ZLIB_TAR_BALL);
	pushd $(ZLIB_NAME)/;\
		export CC=$(OSDRV_CROSS)-gcc;\
		./configure --prefix=zlib_install;\
		make -j 20 > /dev/null;\
		make install > /dev/null;\
	popd

libuuid:
	rm $(UUID_NAME) -rf;
	tar -xf $(UUID_TAR_BALL);
	pushd $(UUID_NAME)/;\
		./configure --host=$(OSDRV_CROSS) --prefix=$(LIBUUID_INSTALL);\
		make -j 20 > /dev/null;\
		make install > /dev/null;\
	popd

clean:
	rm $(TOOL_NAME) -rf;
	rm $(TOOL_BIN) -rf;
	rm $(ZLIB_NAME) -rf;
	rm $(UUID_NAME) -rf;

distclean: clean
