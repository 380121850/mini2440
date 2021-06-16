
INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../../kdesrc/kdecore -I$(srcdir)/../../kdesrc \
	   -I$(srcdir)/../../kdesrc/kssl $(SSL_INCLUDES) $(QTOPIA_INCLUDES) $(all_includes)

noinst_LTLIBRARIES = libkssldropin.la

libkssldropin_la_SOURCES = ksslinfodlg.cpp kopenssl.cpp
libkssldropin_la_METASOURCES = AUTO

KDE_OPTIONS = qtonly
