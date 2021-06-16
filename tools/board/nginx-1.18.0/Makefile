
default:	build

clean:
	rm -rf Makefile objs

build:
	$(MAKE) -f objs/Makefile

install:
	$(MAKE) -f objs/Makefile install

modules:
	$(MAKE) -f objs/Makefile modules

upgrade:
	../../../pub/bin/board/nginx/sbin/nginx -t

	kill -USR2 `cat ../../../pub/bin/board/nginx/logs/nginx.pid`
	sleep 1
	test -f ../../../pub/bin/board/nginx/logs/nginx.pid.oldbin

	kill -QUIT `cat ../../../pub/bin/board/nginx/logs/nginx.pid.oldbin`
