KDE_OPTIONS = qtonly

embedded_inc = -I$(srcdir)/../../dropin -I$(srcdir)/../../kdesrc \
	-I$(srcdir)/../../kdesrc/kdecore -I$(srcdir)/../../kdesrc/khtml

INCLUDES    = $(embedded_inc) -I$(srcdir)/../../src $(all_includes)

noinst_LTLIBRARIES = libjsdebug_e.la
libjsdebug_e_la_SOURCES = jsdebug.cpp

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/jsdebug.pot
