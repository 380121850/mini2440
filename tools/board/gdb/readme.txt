The gdb-7.9.1.tar.gz and ncurses-6.0.tar.gz	is restricted by the GNU GNUGPL (common public license).
The Copyleft of GPL cannot be used in HiSilicon.The download URL is as follows:
gdb：http://ftp.gnu.org/gnu/gdb/
ncurses：http://ftp.gnu.org/gnu/ncurses/
usage:
1) Save the downloaded gdb-7.9.1.tar.gz and ncurses-6.0.tar.gz to the tools/board/gdb directory of the osdrv.
2) On the Linux server, go to the root directory of osdrv and run the following command:
	cd tools/board/gdb
	tar -xzf gdb-7.9.1.tar.gz
	cd gdb-7.9.1
	patch -p1 < ../hi-gdb-7.9.1.patch
	cd ../
	make all
