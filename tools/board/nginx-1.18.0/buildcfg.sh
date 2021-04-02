#! /bin/sh

./configure --prefix=../../../pub/bin/board/nginx --with-http_ssl_module  \
--with-cc=arm-linux-gcc \
--with-cpp=arm-linux-g++ \
--with-pcre=../pcre/pcre-8.44 --with-zlib=../zlib/zlib-1.2.11 --with-openssl=../openssl/openssl-1.1.1j \
--with-openssl-opt="--cross-compile-prefix=arm-linux- linux-armv4" --without-http_upstream_zone_module \
--user=root \
--group=root \
--error-log-path=/var/log/nginx/error.log \
--http-log-path=/var/log/nginx/access.log \
--http-proxy-temp-path=/var/log/nginx/proxy-temp \
--http-fastcgi-temp-path=/var/log/nginx/fastcgi-temp \
--http-uwsgi-temp-path=/var/log/nginx/uwsgi-temp \
--http-scgi-temp-path=/var/log/nginx/scgi-temp \
--http-client-body-temp-path=/var/log/nginx/client-body-temp \
--with-mail_ssl_module \
--without-mail_pop3_module \
--without-mail_imap_module \
--without-mail_smtp_module 


echo "#ifndef NGX_HAVE_SYSVSHM" >> objs/ngx_auto_config.h
echo "#define NGX_HAVE_SYSVSHM 1" >> objs/ngx_auto_config.h
echo "#endif" >> objs/ngx_auto_config.h

echo    >> objs/ngx_auto_config.h

echo "#ifndef NGX_SYS_NERR" >> objs/ngx_auto_config.h
echo "#define NGX_SYS_NERR  132" >> objs/ngx_auto_config.h
echo "#endif" >> objs/ngx_auto_config.h

echo "Sucess!"
