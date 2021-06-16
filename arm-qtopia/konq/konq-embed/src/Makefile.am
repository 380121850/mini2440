# the implementations of preferences.ui are deliberately in
# the CVS repository, to avoid a uic dependency (for now)
# so when making changes to preferences.ui don't forget to
# 'make uifiles'

SUBDIRS = actions

UIC = uic

if LARGEICONS
isearch = pics/big
else
isearch = pics
endif

if EXPORT_SYMBOLS
exportflag = -export-dynamic
else
exportflag =
endif

if DYNAMIC_ADD_ONS
lib_addons = $(LIB_ADDONS)
init_addons= $(INIT_ADDONS)
else
lib_addons = $(LIB_ADDONS) $(LIB_DYN_ADDONS)
init_addons= $(INIT_ADDONS) $(INIT_DYN_ADDONS)
endif

INCLUDES = -I$(srcdir)/$(isearch) \
	   -I$(srcdir)/../dropin -I$(srcdir)/../kdesrc/kdecore \
	   -I$(srcdir)/../kdesrc/kio/http/kcookiejar \
	   -I$(srcdir)/../kdesrc/kssl \
	   -I$(srcdir)/../kdesrc/khtml -I$(srcdir)/../kdesrc \
	   -I$(srcdir)/../kdesrc/khtml/dom \
	   -I$(srcdir)/../kdesrc/khtml/misc \
	   -I$(srcdir)/../dropin/kio \
	   -I$(srcdir)/../dropin/khtml \
	   $(QTOPIA_INCLUDES) $(all_includes)

bin_PROGRAMS = konqueror

konqueror_SOURCES = main.cc mainwindowbase.cc htmlview.cc popuphandler.cc \
	       view.cc run.cc browseriface.cc \
	       bookmarks.cc preferences.cc preferencesimpl.cc \
               mainwindow_qpe.cc mainwindow_x11.cc mainwindow_kiosk.cc \
               xmltree.cc bookmarkeditor.cc bookmarkeditorimpl.cc \
               bookmarkedit.cc bookmarkeditimpl.cc mainwindebug.cc

konqueror_LDADD = $(lib_addons) \
	     $(LIB_KJSHTML) \
	     $(LIB_KHTML)

konqueror_LDFLAGS = $(KONQ_SSL_LDFLAGS) $(all_libraries) $(exportflag)
konqueror_METASOURCES = AUTO
konqueror_DEPENDENCIES = $(LIB_KJSHTML) $(LIB_KHTML) $(lib_addons)
BUILT_SOURCES = init.inc

icon_DATA = konqueror.png
icondir = $(kde_icondir)

dotdesktop_DATA = konqueror.desktop
dotdesktopdir = $(kde_appsdir)/Applications

KDE_OPTIONS = qtonly

init.inc: $(lib_addons)
	echo '/* This file is generated automatically, do not edit */' > $@
	$(if $(init_addons),for i in $(init_addons); do echo "KONQE_INIT($$i);" >> $@ ; done)

uifiles:
	$(UIC) -o $(srcdir)/preferences.h $(srcdir)/preferences.ui
	echo '#include <klocale.h>' > $(srcdir)/preferences.cc
	$(UIC) -tr i18n -i preferences.h $(srcdir)/preferences.ui | sed -e "s,i18n( \"\" ),QString::null,g" >> $(srcdir)/preferences.cc
	echo '#include "preferences.moc"' >> $(srcdir)/preferences.cc

	echo "#include <config.h>" > $(srcdir)/bookmarkeditor.h
	echo "#if defined(ENABLE_BOOKMARKS)" >> $(srcdir)/bookmarkeditor.h
	$(UIC) $(srcdir)/bookmarkeditor.ui >> $(srcdir)/bookmarkeditor.h
	echo "#endif" >> $(srcdir)/bookmarkeditor.h

	echo "#include <config.h>" > $(srcdir)/bookmarkeditor.cc
	echo "#if defined(ENABLE_BOOKMARKS)" >> $(srcdir)/bookmarkeditor.cc
	echo '#include <klocale.h>' >> $(srcdir)/bookmarkeditor.cc
	$(UIC) -tr i18n -i bookmarkeditor.h $(srcdir)/bookmarkeditor.ui | sed -e "s,i18n( \"\" ),QString::null,g" >> $(srcdir)/bookmarkeditor.cc
	echo '#include "bookmarkeditor.moc"' >> $(srcdir)/bookmarkeditor.cc
	echo "#endif" >> $(srcdir)/bookmarkeditor.cc

	echo "#include <config.h>" > $(srcdir)/bookmarkedit.h
	echo "#if defined(ENABLE_BOOKMARKS)" >> $(srcdir)/bookmarkedit.h
	$(UIC) $(srcdir)/bookmarkedit.ui >> $(srcdir)/bookmarkedit.h
	echo "#endif" >> $(srcdir)/bookmarkedit.h

	echo "#include <config.h>" > $(srcdir)/bookmarkedit.cc
	echo "#if defined(ENABLE_BOOKMARKS)" >> $(srcdir)/bookmarkedit.cc
	echo '#include <klocale.h>' >> $(srcdir)/bookmarkedit.cc
	$(UIC) -tr i18n -i bookmarkedit.h $(srcdir)/bookmarkedit.ui | sed -e "s,i18n( \"\" ),QString::null,g" >> $(srcdir)/bookmarkedit.cc
	echo '#include "bookmarkedit.moc"' >> $(srcdir)/bookmarkedit.cc
	echo "#endif" >> $(srcdir)/bookmarkedit.cc

fixup_xpms:
	@cd $(srcdir) ;\
	for xpm in `find pics -type f -name *.xpm`; do \
		if ! grep "static const char \\* const" $$xpm 2>/dev/null >/dev/null; then \
			echo "fixing $$xpm" ;\
			perl -pi -e "s,static const char \\*(.*),static const char * const \\1," $$xpm ;\
		fi ;\
	done
