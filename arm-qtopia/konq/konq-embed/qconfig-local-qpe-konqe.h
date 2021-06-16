#ifndef QT_H
#endif // QT_H

// If you are using this file with Qt 2.3.1 then you have to apply the
// qte231-for-qpe.patch from the QPE 1.3.1 release (contained in the qt/
// subdirectory) to your Qt, independent from whether you really use QPE
// or not. (Simon)

// Empty leaves all features enabled.  See doc/html/features.html for choices.

// Note that disabling some features will produce a libqt that is not
// compatible with other libqt builds. Such modifications are only
// supported on Qt/Embedded where reducing the library size is important
// and where the application-suite is often a fixed set.

#ifndef QT_DLL
#define QT_DLL // Internal
#endif

#define QT_NO_PROCESS
#if defined(QT_QWS_IPAQ) || defined(QT_QWS_CASSIOPEIA)
# define QT_NO_QWS_CURSOR
# define QT_NO_QWS_MOUSE_AUTO
#endif
#ifndef QT_NO_CODECS
#define QT_NO_CODECS
#endif
#define QT_NO_UNICODETABLES
#define QT_NO_IMAGEIO_BMP
#define QT_NO_IMAGEIO_PPM
//#define QT_NO_ASYNC_IO
//#define QT_NO_ASYNC_IMAGE_IO
#define QT_NO_FREETYPE
#define QT_NO_BDF
#define QT_NO_FONTDATABASE
#define QT_NO_DRAGANDDROP
#define QT_NO_CLIPBOARD
#define QT_NO_PROPERTIES
#define QT_NO_NETWORKPROTOCOL

#define QT_NO_IMAGE_TEXT

//#define QT_NO_TOOLTIP
#define QT_NO_COLORNAMES
#define QT_NO_TRANSFORMATIONS
#define QT_NO_PRINTER
#define QT_NO_PICTURE
//#define QT_NO_ICONVIEW
#define QT_NO_DIAL
#define QT_NO_SIZEGRIP
#define QT_NO_WORKSPACE
//#define QT_NO_TABLE
//#define QT_NO_ACTION
#define QT_NO_STYLE_MOTIF
#define QT_NO_STYLE_PLATINUM
#define QT_NO_FILEDIALOG
#define QT_NO_FONTDIALOG
#define QT_NO_PRINTDIALOG
#define QT_NO_COLORDIALOG
#define QT_NO_INPUTDIALOG
//#define QT_NO_MESSAGEBOX
#define QT_NO_PROGRESSDIALOG
//#define QT_NO_TABDIALOG
#define QT_NO_WIZARD
#define QT_NO_EFFECTS
#define QT_NO_PLUGIN
#define QT_NO_COMPONENT //renamed from QT_NO_PLUGIN  for Qt 3.0
#define QT_NO_DOM
#define QT_NO_SEMIMODAL
//#define QT_NO_PROGRESSBAR
#define QT_NO_SPLITTER

#define QT_NO_QWS_SAVEFONTS
#define QT_NO_QWS_PROPERTIES

#define QT_NO_QWS_BEOS_WM_STYLE
#define QT_NO_QWS_KDE2_WM_STYLE
#define QT_NO_QWS_KDE_WM_STYLE
#define QT_NO_QWS_WINDOWS_WM_STYLE

