#ifndef __kstdaction_h__
#define __kstdaction_h__

class QObject;
class QString;
class KAction;
class KToggleAction;

class KStdAction
{
public:
    enum StdAction {
	New=1, Open, Save, SaveAs, Close, Print, Quit,
	Undo, Redo, Cut, Copy, Paste, SelectAll, Find,
	Zoom, Redisplay,
	Back, Forward, Home,
	AddBookmark, EditBookmarks,
	ShowToolbar, ShowStatusbar,
	Preferences,
	Help,
	_std_last
    };

    static KAction *action( StdAction act_enum, const QObject *recvr = 0,
                	    const char *slot = 0, QObject *parent = 0,
                	    const char *name = 0L );

    static const char* stdName( StdAction act_enum );

    static QString stdText( int act_enum );
    static bool stdToggle( int act_enum );
    static int stdAccel( int act_enum );
    static int stdAction( const char *name );

#define MKACTION(fname,ename) \
    static KAction *fname( const QObject *receiver = 0, const char *slot = 0, \
                           QObject *parent = 0, const char *name = 0 ) \
	{ return action( ename, receiver, slot, parent, name ); }

#define MKTOGGLEACTION(fname,ename) \
    static KToggleAction *fname( const QObject *receiver = 0, const char *slot = 0, \
                           QObject *parent = 0, const char *name = 0 ) \
	{ return (KToggleAction*)action( ename, receiver, slot, parent, name ); }

    MKACTION(openNew,New)
    MKACTION(open,Open)
    MKACTION(save,Save)
    MKACTION(saveAs,SaveAs)
    MKACTION(close,Close)
    MKACTION(print,Print)
    MKACTION(quit,Quit)
    MKACTION(undo,Undo)
    MKACTION(redo,Redo)
    MKACTION(cut,Cut)
    MKACTION(copy,Copy)
    MKACTION(paste,Paste)
    MKACTION(selectAll,SelectAll)
    MKACTION(find,Find)
    MKACTION(zoom,Zoom)
    MKACTION(redisplay,Redisplay)
    MKACTION(back,Back)
    MKACTION(forward,Forward)
    MKACTION(home,Home)
    MKACTION(addBookmark,AddBookmark)
    MKACTION(editBookmarks,EditBookmarks)
    MKTOGGLEACTION(showToolbar,ShowToolbar);
    MKTOGGLEACTION(showStatusbar,ShowStatusbar);
    MKACTION(preferences,Preferences)
    MKACTION(help,Help)

#undef MKACTION
#undef MKTOGGLEACTION
};

#endif
