INCLUDES= -I$(srcdir)/../../dropin -I$(srcdir)/../../dropin/kio \
          -I$(srcdir)/../kdecore -I$(srcdir)/../../dropin/kssl \
          $(SSL_INCLUDES) $(QTOPIA_INCLUDES) $(all_includes)

noinst_LTLIBRARIES = libkssl.la
KSSLVERSION=	2.0.0
KSSLPATCH=	0
METASOURCES= AUTO


noinst_HEADERS = \
		kssl.h                   \
		ksslcertificatehome.h    \
		ksslsettings.h           \
		ksslcertificate.h        \
		ksslconnectioninfo.h     \
		ksslcertificatefactory.h \
		ksslcertificatecache.h   \
		ksslpeerinfo.h           \
                ksslcertdlg.h            \
		ksslutils.h              \
	        ksslcertchain.h          \
		ksslx509map.h            \
                ksslpkcs12.h             \
                kssld.h
 

libkssl_la_SOURCES	= \
			kssl.cc                   \
			ksslcertificatehome.cc    \
			ksslsettings.cc           \
			ksslcertificate.cc        \
			ksslconnectioninfo.cc     \
			ksslcertificatefactory.cc \
			ksslcertificatecache.cc   \
			ksslpeerinfo.cc           \
			ksslcertdlg.cc            \
			ksslutils.cc              \
			ksslcertchain.cc          \
			ksslx509map.cc            \
			ksslpkcs12.cc             \
                        kssld.cpp                 \
                        kssld_skel.cpp

libkssl_la_METASOURCES = AUTO

SUBDIRS = kssl
KDE_OPTIONS = qtonly
