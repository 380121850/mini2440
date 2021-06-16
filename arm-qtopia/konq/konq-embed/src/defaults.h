#ifndef __defaults_h__
#define __defaults_h__

/* Site-specific default values */

#define DEFAULT_HOMEPAGE	"http://www.kde.org/"
#define DEFAULT_WINSTYLE	MainWindowBase::STYLE_WINDOWED
#define DEFAULT_STATUSDELAY	5000
#define DEFAULT_STATUSBAR	TRUE
#define DEFAULT_LOCATIONBAR	TRUE
#define DEFAULT_SIDEBAR		TRUE
#define DEFAULT_ZOOM		100

#if defined (USE_LARGE_ICONS)
#define DEFAULT_BIGPIXMAPS	TRUE
#else
#define DEFAULT_BIGPIXMAPS	FALSE
#endif

#endif
