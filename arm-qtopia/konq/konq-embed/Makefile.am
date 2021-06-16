SUBDIRS = dropin kdesrc add-ons src
DESTDIR = /tmp/konq-ipkg

khtml_DATA = khtmlrc
khtmldir = $(kde_confdir)

messages:
	rm -f i18nfiles
	find $(srcdir) -type f -name "*.cpp" > i18nfiles
	find $(srcdir) -type f -name "*.cc" >> i18nfiles
	$(XGETTEXT) -C -ki18n -kI18N_NOOP `cat ./i18nfiles` -o ./konqueror.pot
	rm -f i18nfiles

ipkg_doit:
	rm -rf ${DESTDIR}
	$(MAKE) install DESTDIR=${DESTDIR} transform="s,x,x,"
	$(MAKE) -C ipkg ipkg_doit DESTDIR=${DESTDIR} MKIPKG_USER_OPTS="${MKIPKG_USER_OPTS}"
	rm -rf ${DESTDIR}

