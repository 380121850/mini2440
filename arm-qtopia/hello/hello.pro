CONFIG		+= qtopiaapp
CONFIG		-= buildQuicklaunch
DESTDIR	= $(QPEDIR)/bin

HEADERS	= hello.h
SOURCES	= hello.cpp

SOURCES+=main.cpp

INTERFACES	= hello_base.ui
TARGET		= hello
