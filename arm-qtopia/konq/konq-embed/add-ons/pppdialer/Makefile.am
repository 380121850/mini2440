SUBDIRS = pics

KDE_OPTIONS = qtonly

UIC = uic

if LARGEICONS
isearch = pics/big
else
isearch = pics
endif

INCLUDES = -I$(srcdir)/$(isearch) \
	-I$(srcdir)/../../dropin -I$(srcdir)/../../kdesrc \
	-I$(srcdir)/../../kdesrc/kdecore -I$(srcdir)/../../src $(all_includes)

noinst_LTLIBRARIES = libpppdialer_e.la
libpppdialer_e_la_SOURCES = pppdialer.cpp dialer.cpp

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

uifiles:
	$(UIC) -o $(srcdir)/dialer.h $(srcdir)/dialer.ui
	echo '#include <klocale.h>' > $(srcdir)/dialer.cpp
	$(UIC) -tr i18n -i dialer.h $(srcdir)/dialer.ui | sed -e "s,i18n( \"\" ),QString::null,g" >> $(srcdir)/dialer.cpp
	echo '#include "dialer.moc"' >> $(srcdir)/dialer.cpp

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/pppdialer.pot

fixup_xpms:
	for xpm in `find $(srcdir)/pics -type f -name '*.xpm'`; do \
		if ! grep 'static const char \* const' $$xpm 2>/dev/null >/dev/null; then \
			echo "fixing `basename $$xpm`" ;\
			perl -pi -e 's,static( const)? char \*(.*),static const char * const \2,' $$xpm ;\
		fi ;\
	done
