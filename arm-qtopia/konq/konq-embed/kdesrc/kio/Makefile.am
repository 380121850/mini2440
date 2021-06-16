
SUBDIRS = . http

INCLUDES = -I$(srcdir)/../../dropin -I$(srcdir)/../../dropin/kio \
	   -I$(srcdir)/..  -I$(srcdir)/../kdecore -I$(srcdir)/../kssl $(SSL_INCLUDES) \
           $(QTOPIA_INCLUDES) $(all_includes)

noinst_LTLIBRARIES = libkio.la libkio_ftp.la

libkio_la_SOURCES = connection.cpp slaveinterface.cpp authinfo.cpp tcpslavebase.cpp
libkio_la_LIBADD = $(KIO_FTP)
libkio_la_DEPENDENCIES = libkio_ftp.la
libkio_la_METASOURCES = AUTO

libkio_ftp_la_SOURCES = ftp.cc

KDE_OPTIONS = qtonly
