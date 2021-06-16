# Automake source file for KPlainText Part

if KONQ_EMBEDDED
embedded_inc = -I$(srcdir)/../../dropin -I$(srcdir)/../../kdesrc \
	-I$(srcdir)/../../kdesrc/kdecore
kplaintext_emb = libkplaintext_e.la
kplaintext_kde =
else
embedded_inc =
kplaintext_emb =
kplaintext_kde = libkplaintextpart.la
endif

# set the include path for X, qt and KDE
INCLUDES = $(embedded_inc) $(all_includes)

# these are the headers for your project
noinst_HEADERS = kplaintext_part.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kplaintext.pot

KDE_ICON = kplaintext

#########################################################################
# KPART SECTION
#########################################################################

# Full Desktop version
kde_module_LTLIBRARIES = $(kplaintext_kde)
libkplaintextpart_la_SOURCES = kplaintext_part.cpp
libkplaintextpart_la_LDFLAGS = -module $(KDE_PLUGIN)
libkplaintextpart_la_LIBADD  = $(LIB_KPARTS) $(all_libraries)

# Embedded version
noinst_LTLIBRARIES = $(kplaintext_emb)
libkplaintext_e_la_SOURCES = kplaintext_part.cpp

# this is where the desktop file will go 
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = kplaintextpart.desktop

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/kplaintextpart
partrc_DATA = kplaintextpart.rc
