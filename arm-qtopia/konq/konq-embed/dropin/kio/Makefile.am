
SUBDIRS = kded

INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../../kdesrc/kdecore -I$(srcdir)/../../kdesrc \
	   -I$(srcdir)/../../kdesrc/kio/http -I$(srcdir)/../../kdesrc/kio -I../../kdesrc/kio/http \
	   -I$(srcdir)/../../kdesrc/kssl -I$(srcdir)/../../kdesrc/khtml -I$(srcdir)/../khtml \
	   $(QTOPIA_INCLUDES) $(all_includes)

noinst_LTLIBRARIES = libkiodropin.la

libkiodropin_la_SOURCES = slavebase.cpp job.cpp netaccess.cpp jobclasses.cpp \
			  ktrader.cpp kprotocolmanager.cpp krun.cpp authcache.cpp authcache_skel.cpp \
			  scheduler.cpp observer.cpp uiserver.cpp uiserver_skel.cpp \
			  slave.cpp passdlg.cpp kio_file.cpp kio_ftp.cpp \
			  kmimemagic.cpp global.cpp \
	                  launcher.cpp slaveconfig.cpp mimehandler.cpp kmimetype.cpp
libkiodropin_la_METASOURCES = AUTO

dcopskel:
	dcopidl authcache.h > authcache.kidl || ( rm -f authcache.kidl ; /bin/false )
	dcopidl2cpp --c++-suffix cpp --no-stub authcache.kidl
	rm authcache.kidl
	dcopidl uiserver.h > uiserver.kidl || ( rm -f uiserver.kidl ; /bin/false )
	dcopidl2cpp --c++-suffix cpp --no-stub uiserver.kidl
	rm uiserver.kidl

KDE_OPTIONS = qtonly
