#ifndef __BOOKMARKS_H__
#define __BOOKMARKS_H__

#include <config.h>

#if defined(ENABLE_BOOKMARKS)

#include <qaction.h>
#include <qlist.h>
#include <qstring.h>
#include <qpopupmenu.h>

#include <kurl.h>

class QPopupMenu;
class XMLElement;
class ActionMenu;
class KAction;

class BookmarkAction: public QAction
{
    Q_OBJECT

  public:

    BookmarkAction( QString title, QString url, QObject* parent = 0 );

  signals:

    void activated( const QString& url );

  protected slots:

    void selfActivated();

  protected:

    QString m_title, m_url;

 private:
    // no copy
    BookmarkAction( const BookmarkAction& );
    BookmarkAction& operator =( const BookmarkAction& );

};

class BookmarkFolder;
class Bookmark
{
    friend class BookmarkFolder;

  public:

    Bookmark( XMLElement *elem = 0 ) : element( elem ) {}

    Bookmark( const Bookmark &other ) { (*this) = other; }

    Bookmark &operator=( const Bookmark &other ) { element = other.element; return *this; }

    bool isNull() const;

    bool isSeparator() const;
    
    bool isFolder() const;

    void setTitle( const QString &title );

    QString title() const;

    QString menuTitle() const;

    void setURL( const QString &url );

    QString url() const;

    BookmarkFolder toFolder() const;

    XMLElement *xmlElement() const { return element; }

  protected:

    XMLElement *element;

};

class BookmarkFolder: public Bookmark
{

  public:

    BookmarkFolder () : Bookmark( 0 ) {}

    BookmarkFolder ( XMLElement *elem ) : Bookmark( elem ) {}

    BookmarkFolder( const BookmarkFolder &other ) : Bookmark( 0 ) { (*this) = other; }

    BookmarkFolder &operator=( const BookmarkFolder &other ) { Bookmark::operator=( other );  return *this; }

    Bookmark first() const;

    Bookmark previous( const Bookmark& current ) const;

    Bookmark next( const Bookmark& next ) const;

  private:

    XMLElement *nextXMLElement( XMLElement *start ) const;

    XMLElement *prevXMLElement( XMLElement *start ) const;

};

class BookmarkMenu: public QPopupMenu
{
     Q_OBJECT

  public:

    BookmarkMenu( const BookmarkFolder &top );
    ~BookmarkMenu();

    void refill( const BookmarkFolder &top );

  signals:

    void activatedBookmark( const QString& url );

  protected slots:

    void slotActivated( const QString& url );

  protected:
  
    void clearMenu();

    void fillBookmarkMenu( const BookmarkFolder &top );

    BookmarkFolder m_top;

    QList<BookmarkAction> m_actions;

    QList<BookmarkMenu> m_subMenus;

};

class MainWindowBase;
class QAction;

class BookmarkManager : public QObject
{
    Q_OBJECT
public:
    BookmarkManager( MainWindowBase *mainWindow );
    virtual ~BookmarkManager();

    bool addTo( QWidget *w );

    void enable( bool enabled );

    inline bool isAvailable() const { return true; }

public slots:
    void bookmarkSelected( const QString & );

    void addBookmark();

    void addBookmark( const KURL &url, const QString &title = QString::null );

    void editBookmarks();

private:
    void loadBookmarks();

    void saveBookmarks();

    void createBookmarksMenu();

    KAction *m_add;
    KAction *m_edit;
    ActionMenu *m_actionMenu;
    BookmarkMenu *m_bookmarkMenu;

    QString m_bookmarkFile;
    XMLElement *m_xbeldoc;

    MainWindowBase *m_mainWindow;
};

#else

class BookmarkManager : public QObject
{
public:
    inline BookmarkManager( QObject *parent )
        : QObject( parent ) {}

    inline void addBookmark( const KURL &, const QString & = QString::null ) {}

    inline bool addTo( QWidget * ) { return false;}

    inline void enable( bool ) {}

    inline bool isAvailable() const { return false; }
};

#endif // ENABLE_BOOKMARKS

#endif // __BOOKMARKS_H__

/* vim: et sw=4
 */
