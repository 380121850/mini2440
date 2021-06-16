# Automake source file for KSimpleSound Part

if KONQ_EMBEDDED
embedded_inc = -I$(srcdir)/../../dropin -I$(srcdir)/../../kdesrc \
	-I$(srcdir)/../../kdesrc/kdecore
ksimplesound_emb = libksimplesound_e.la
ksimplesound_kde =
else
embedded_inc =
ksimplesound_emb =
ksimplesound_kde = libksimplesound.la
endif

# set the include path for X, qt and KDE
INCLUDES = $(embedded_inc) $(all_includes)

# these are the headers for your project
noinst_HEADERS = ksimplesound.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ksimplesound.pot

KDE_ICON = ksound

#########################################################################
# KPART SECTION
#########################################################################

# Full Desktop version
kde_module_LTLIBRARIES = $(ksimplesound_kde)
libksimplesound_la_SOURCES = ksimplesound.cpp
libksimplesound_la_LDFLAGS = -module $(KDE_PLUGIN)
libksimplesound_la_LIBADD  = $(LIB_KPARTS) $(all_libraries)

# Embedded version
noinst_LTLIBRARIES = $(ksimplesound_emb)
libksimplesound_e_la_SOURCES = ksimplesound.cpp

# this is where the desktop file will go 
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = ksimplesound.desktop

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/ksimplesound
partrc_DATA = ksimplesound.rc
