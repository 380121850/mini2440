#!/bin/bash

DESTDIR=/tmp/FriendlyARM/mini2440/kernel
rm -rf ${DESTDIR}
mkdir -p ${DESTDIR}

touch .scmversion
for suffix in a70 a70i h43 l80 n35 n43 p35 p43 s70 t35 td35 vga1024x768 vga640x480 vga800x600 w35 x35; do
	cp config_mini2440_${suffix} .config && \
		make -j6 && \
		cp -vf arch/arm/boot/zImage ${DESTDIR}/zImage_${suffix^^} || exit 1
done

echo "done!"
ls -l /tmp/FriendlyARM/mini2440/kernel/*