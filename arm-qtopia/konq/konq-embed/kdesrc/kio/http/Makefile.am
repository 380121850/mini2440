
AM_CXXFLAGS = -DMAX_CACHE_AGE=300

INCLUDES = -I$(srcdir)/../../../dropin -I$(srcdir)/../../kdecore -I$(srcdir)/../.. \
	   -I$(srcdir)/../../../dropin/kio -I$(srcdir)/../../kssl \
	   -I$(srcdir)/../ $(QTOPIA_INCLUDES) $(all_includes)

SUBDIRS = kcookiejar

noinst_LTLIBRARIES = libkiohttp.la

libkiohttp_la_SOURCES = http.cc http_cache_cleaner.cpp
libkiohttp_la_LIBADD = ../libkio.la ../../../dropin/kio/libkiodropin.la

KDE_OPTIONS = qtonly
