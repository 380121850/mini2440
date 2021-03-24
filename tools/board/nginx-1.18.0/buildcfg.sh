#! /bin/sh

./configure --prefix=./objs/_install --with-http_ssl_module --with-openssl=/home/openssl-1.0.2s \
--with-cc=arm-linux-gcc \
--with-cpp=arm-linux-g++ \
--with-pcre=../pcre/pcre-8.44 --with-zlib=../zlib/zlib-1.2.11 --with-openssl=../openssl/openssl-1.1.1j \
--with-openssl-opt="--cross-compile-prefix=arm-linux- linux-armv4" --without-http_upstream_zone_module 

echo "#ifndef NGX_HAVE_SYSVSHM" >> objs/ngx_auto_config.h
echo "#define NGX_HAVE_SYSVSHM 1" >> objs/ngx_auto_config.h
echo "#endif" >> objs/ngx_auto_config.h

echo    >> objs/ngx_auto_config.h

echo "#ifndef NGX_SYS_NERR" >> objs/ngx_auto_config.h
echo "#define NGX_SYS_NERR  132" >> objs/ngx_auto_config.h
echo "#endif" >> objs/ngx_auto_config.h

echo "Sucess!"
