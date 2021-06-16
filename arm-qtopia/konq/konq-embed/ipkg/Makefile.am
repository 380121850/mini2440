
MKIPKG_OPTS=--destdir=${DESTDIR} --srcdir=${srcdir} \
            --strip=${STRIP} --builddir=`pwd` \
	    --prefix=${prefix}

ipkg_doit: install
	$(SHELL) $(srcdir)/mkipkg $(MKIPKG_OPTS) --control=$(srcdir)/control.in \
	          --preinst=preinst $(MKIPKG_USER_OPTS)


config_DATA=konq-embedrc
configdir=$(kde_confdir)

