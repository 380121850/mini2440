INCLUDES = -I$(srcdir)/../ -I$(srcdir)/../../kdesrc/kdecore -I$(srcdir)/../../kdesrc \
	   -I$(srcdir)/../../kdesrc/khtml -I$(srcdir)/../../kdesrc/khtml/xml \
	   -I$(srcdir)/../../kdesrc/khtml/dom -I$(srcdir)/../../dropin/kio \
	   $(QTOPIA_INCLUDES) $(all_includes)

noinst_LTLIBRARIES = libkhtmldropin.la

libkhtmldropin_la_SOURCES = khtml_ext.cpp khtml_run.cpp
libkhtmldropin_la_METASOURCES = AUTO


KDE_OPTIONS = qtonly
