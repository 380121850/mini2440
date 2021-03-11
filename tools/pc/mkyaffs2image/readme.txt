The yaffs2utils0.2.9 is restricted by the GNU GNUGPL (common public license).
The Copyleft of GPL cannot be used in HiSilicon.The download URL is as follows:
https://github.com/dorigom/yaffs2utils/releases
usage:
1) Save the downloaded yaffs2utils-0.2.9.tar.gz to the tools/pc/mkyaffs2image directory of the osdrv.
2) On the Linux server, go to the root directory of osdrv and run the following command:
	cd tools/pc/mkyaffs2image
	tar -xzf yaffs2utils-0.2.9.tar.gz
	cd yaffs2utils-0.2.9
	patch -p1 < ../hi_yaffs2utils.patch
	cd ../
	make
