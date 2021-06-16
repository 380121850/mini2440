INCLUDES = -I$(srcdir)/../kdesrc/kdecore -I$(srcdir)/../kdesrc $(QTOPIA_INCLUDES) \
           $(all_includes)

SUBDIRS = . kio kparts khtml kssl

noinst_LTLIBRARIES = libdropin.la

libdropin_la_SOURCES = kdebug.cpp kglobal.cpp kconfig.cpp kapp.cpp dcopobject.cpp \
	kcmdlineargs.cpp dcopclient.cpp kdialogbase.cpp kuniqueapp.cpp \
	kdialog.cpp kinstance.cpp klibloader.cpp dcopdispatcher.cpp \
	kaction.cpp kstdaction.cpp kglobalsettings.cpp \
	kiconloader.cpp kmessagebox.cpp fakes.cpp kbuttonbox.cpp \
	kfiledialog.cpp kcombobox.cpp klineedit.cpp klistbox.cpp \
	ksocks.cpp kcharsets.cpp kinputdialog.cpp kwin.cpp \
	klocale.cpp jsextension.cpp
libdropin_la_METASOURCES = AUTO


KDE_OPTIONS = qtonly
