#!/bin/sh
# this script needs to be executed after Makefile.cvs on unix, to
# remove some stuff

files_to_remove="./kdesrc/kdecore/ksock.h ./kdesrc/kdecore/ksock.cpp ./kdesrc/kio/connection.h"
files_to_remove="$files_to_remove ./kdesrc/kio/connection.cpp"
files_to_remove="$files_to_remove ./kdesrc/kio/tcpslavebase.cpp"
files_to_remove="$files_to_remove ./kdesrc/kio/tcpslavebase.h ./kdesrc/kdecore/netsupp.h"
files_to_remove="$files_to_remove ./kdesrc/kdecore/kextsock.h "

usage()
{
    echo "usage: $0 [path to konq-embed/ directory]"
    exit 1
}

if [ $# != 1 ]; then
    usage
fi

basepath=$1

if [ ! -e $basepath ]; then
    usage
fi

cd $basepath

for i in `ls $files_to_remove`; do
    echo "removing $i"
    rm $i
    if [ $? != 0 ]; then
	echo "error removing $i, exiting..."
	exit 1
    fi
done

for i in `find ./win32/patches/ -type f -maxdepth 1`; do
    echo "applying patch $i..."
    patch -p0 < $i
    if [ $? != 0 ]; then
	echo "error applying patch, exiting..."
	exit 1
    fi
done

echo "adding kio/connection.h hook..."
echo "#include <win32/connection.h>" > ./kdesrc/kio/connection.h

echo "adding kio/tcpslavebase.h hook..."
echo "#include <win32/tcpslavebase.h>" > ./kdesrc/kio/tcpslavebase.h

echo "creating dummy .moc files..."

mkdir -p ./win32/mocdummy

for i in `cat konq.pro| grep .cpp | sed "s,\\\\\\\\,," | sed "s,SOURCES.*=,,"`; do
    file=`basename $i .cpp`
    mocname=$file.moc
    if [ ! -e win32/mocdummy/$mocname ]; then
        grep $mocname $i 2>/dev/null 1>/dev/null
        if [ $? = 0 ]; then
  	    echo "creating $mocname"
	    touch win32/mocdummy/$mocname
        fi
    fi
done

