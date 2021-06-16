#include <config.h>

#if defined(ENABLE_BOOKMARKS)

#include "bookmarks.h"
#include "xmltree.h"
#include "mainwindowbase.h"
#include "bookmarkeditorimpl.h"
#include "view.h"
#include "htmlview.h"

#include <qaction.h>
#include <qlist.h>
#include <qpopupmenu.h>
#include <qregexp.h>
#include <qstring.h>
#include <qwidget.h>
#include <qfile.h>

#include <klocale.h>
#include <kstddirs.h>

#include <assert.h>

BookmarkAction::BookmarkAction( QString title, QString url, QObject* parent ):
  QAction( parent ), m_title( title ), m_url( url )
{
  setText( m_title );
  setStatusTip( m_url );
  connect( this, SIGNAL( activated() ), SLOT( selfActivated() ) );
}

void BookmarkAction::selfActivated()
{
  emit activated( m_url );
}

bool Bookmark::isFolder() const
{
  if ( !element )
    return false;
  QString tag = element->tagName();
  return ( tag == "xbel" || tag == "folder" );
}

bool Bookmark::isNull() const
{
  return element == 0;
}

bool Bookmark::isSeparator() const
{
  if ( !element )
    return false;
  QString tag = element->tagName();
  return ( tag == "separator" );
}

void Bookmark::setTitle( const QString &title )
{
    if ( !element || isSeparator() || isFolder() )
        return;

    XMLElement *titleElement = element->namedItem( "title" );
    if ( !titleElement )
    {
        titleElement = new XMLElement;
        titleElement->setTagName( "title" );
        element->appendChild( titleElement );
    }

    if ( !titleElement->firstChild() )
        titleElement->appendChild( new XMLElement );

    titleElement->firstChild()->setValue( title );
}

QString Bookmark::title() const
{
  if ( !element )
    return QString::null;

  if ( isSeparator() )
      return QString::fromLatin1( "-" );

  XMLElement *titleXMLElement = element->namedItem( "title" );
  if ( !titleXMLElement || !titleXMLElement->firstChild() )
    return QString::null;

  return titleXMLElement->firstChild()->value();
}

QString Bookmark::menuTitle() const
{
  const uint maxlen = 32;
  QString res = title();
  if( res.length() > maxlen )
    res = res.left( maxlen ).append( "..." );
  res.replace( QRegExp( "&" ), "&&" );
  return res;
}

void Bookmark::setURL( const QString &url )
{
    if ( !element )
        return;

    element->attributes()[ "href" ] = url;
}

QString Bookmark::url() const
{
  if ( !element )
    return QString::null;
  return element->attributes()[ "href" ];
}

BookmarkFolder Bookmark::toFolder() const
{
  return BookmarkFolder( element );
}

Bookmark BookmarkFolder::first() const
{
  if ( !element )
    return Bookmark();

  return Bookmark( nextXMLElement( element->firstChild() ) );
}

Bookmark BookmarkFolder::previous( const Bookmark& current ) const
{
  if ( !current.element )
    return Bookmark();

  return Bookmark( prevXMLElement( current.element->prevChild() ) );
}

Bookmark BookmarkFolder::next( const Bookmark& current ) const
{
  if ( !current.element )
    return Bookmark();

  return Bookmark( nextXMLElement( current.element->nextChild() ) );
}

XMLElement *BookmarkFolder::nextXMLElement( XMLElement *start ) const
{
  XMLElement *elem = start;
  while( elem )
  {
    QString tag = elem->tagName();
    if( tag == "folder" || tag == "bookmark" || tag == "separator" )
      break;
    elem = elem->nextChild();
  } 
  return elem;
}

XMLElement *BookmarkFolder::prevXMLElement( XMLElement *start ) const
{
  XMLElement *elem = start;
  while( !elem )
  {
    QString tag = elem->tagName();
    if( tag == "folder" || tag == "bookmark" || tag == "separator" )
      break;
    elem = elem->prevChild();
  }
  return elem;
}

BookmarkMenu::BookmarkMenu( const BookmarkFolder &top )
{
  m_actions.setAutoDelete( TRUE );
  m_subMenus.setAutoDelete( TRUE );

  refill( top );
}

BookmarkMenu::~BookmarkMenu()
{
  clearMenu();
}

void BookmarkMenu::clearMenu()
{
  QListIterator<BookmarkAction> it( m_actions );
  for (; it.current(); ++it )
    it.current()->removeFrom( this );
  m_actions.clear();
  m_subMenus.clear();
}

void BookmarkMenu::refill( const BookmarkFolder &top )
{
  clearMenu();
  fillBookmarkMenu( top );
}

void BookmarkMenu::fillBookmarkMenu( const BookmarkFolder &top )
{
  m_top = top;

  for( Bookmark bm = m_top.first(); !bm.isNull(); bm = m_top.next( bm ) )
  {
    if( !bm.isFolder() )
	{
	  if( !bm.isSeparator() )
      {
        BookmarkAction *action = 
		new BookmarkAction( bm.menuTitle(), bm.url() );
        action->addTo( this );
        m_actions.append( action );
        connect( action, SIGNAL( activated( const QString& ) ), 
                 SLOT( slotActivated( const QString& ) ) );
	  }
	  else insertSeparator();
	}
    else
	{
// let's skip sub-folders for small resolutions (Simon)
#if !defined(KONQ_GUI_QPE)
	  BookmarkMenu *b = new BookmarkMenu( bm.toFolder() );
	  insertItem( bm.menuTitle(), b );
	  connect( b, SIGNAL( activatedBookmark( const QString& ) ),
               SLOT( slotActivated( const QString& ) ) );
	  m_subMenus.append( b );
#endif
	}
  }

}

void BookmarkMenu::slotActivated( const QString& url )
{
  emit activatedBookmark( url );
}

class ActionMenu : public QAction
{
public:
    ActionMenu( QPopupMenu *popup, QObject *parent, const char *name = 0 );

    virtual bool addTo( QWidget *w );
    virtual bool removeFrom( QWidget *w );

private: 
    QPopupMenu *m_popup;
    int m_id;
};

ActionMenu::ActionMenu( QPopupMenu *popup, QObject *parent, const char *name )
    : QAction( parent, name ), m_popup( popup ), m_id( -1 )
{
}

bool ActionMenu::addTo( QWidget *w )
{
    assert( m_id == -1 );
    if ( !w->inherits( "QPopupMenu" ) )
        return false;

    QPopupMenu *menu = static_cast<QPopupMenu *>( w );
    m_id = menu->insertItem( menuText(), m_popup );
    return true;
}

bool ActionMenu::removeFrom( QWidget *w )
{
    assert( m_id != -1 );
    if ( !w->inherits( "QPopupMenu" ) )
        return false;

    QPopupMenu *menu = static_cast<QPopupMenu *>( w );
    menu->removeItem( m_id );
    return true;
}

BookmarkManager::BookmarkManager( MainWindowBase *mainWindow )
    : QObject( mainWindow ), m_mainWindow( mainWindow )
{
    m_add = new KAction( i18n( "Add Bookmark" ), CTRL+Key_B, this, SLOT( addBookmark() ),
			 m_mainWindow->actionCollection(), "bookmark_add" );
    m_edit = new KAction( i18n( "Edit Bookmarks..." ), 0, this, SLOT( editBookmarks() ),
			  m_mainWindow->actionCollection(), "bookmark_edit" );

    m_bookmarkFile = locateLocal( "data", QString::fromLatin1( "konq-embed/bookmarks.xml" ) );
    m_bookmarkMenu = 0;
    m_xbeldoc = 0;
    loadBookmarks();
    createBookmarksMenu();

    m_actionMenu = new ActionMenu( m_bookmarkMenu, this, "bookmarks" );
    m_actionMenu->setMenuText( i18n( "Bookmarks" ) );
}

BookmarkManager::~BookmarkManager()
{
    delete m_bookmarkMenu;
    delete m_xbeldoc;
}

bool BookmarkManager::addTo( QWidget *w )
{
    return m_actionMenu->addTo( w );
}

void BookmarkManager::enable( bool enabled )
{
    m_add->setEnabled( enabled );
    m_edit->setEnabled( enabled );
}

void BookmarkManager::bookmarkSelected( const QString &address )
{
    m_mainWindow->urlEntered( address );
}

void BookmarkManager::addBookmark()
{
    View *view = m_mainWindow->currentView();

    assert( view && view->part() );

    addBookmark( view->part()->url(), view->caption() );
}

void BookmarkManager::addBookmark( const KURL &url, const QString &title )
{
    QString caption = title;
    if ( caption.isEmpty() )
        caption = url.prettyURL();

    XMLElement *docElem = m_xbeldoc->firstChild();
    if ( !docElem ) 
        return; 

    XMLElement *elem = new XMLElement;
    elem->setTagName( "bookmark" );
    docElem->appendChild( elem );

    Bookmark bm( elem );
    bm.setURL( url.url() );
    bm.setTitle( title ); 

    saveBookmarks();
    createBookmarksMenu();
}

void BookmarkManager::editBookmarks()
{
    assert( m_xbeldoc );

    XMLElement *copyOfBookmarks = m_xbeldoc->clone();

    BookmarkEditor *editor = new BookmarkEditor( copyOfBookmarks, m_mainWindow );
    if ( editor->exec() == QDialog::Accepted )
    {
        delete m_xbeldoc;
        m_xbeldoc = editor->donateBookmarks();
        saveBookmarks();
        createBookmarksMenu();
    }
    delete editor;
}

void BookmarkManager::loadBookmarks()
{
    delete m_xbeldoc;
    m_xbeldoc = XMLElement::load( m_bookmarkFile );

    if ( !m_xbeldoc )
    {
        m_xbeldoc = new XMLElement;
        m_xbeldoc->setTagName( "!DOCTYPE BOOKMARKS" );
        XMLElement *xbel = new XMLElement;
        xbel->setTagName( "xbel" );
        m_xbeldoc->appendChild( xbel );
    }
    else
        m_xbeldoc->setTagName( "!DOCTYPE BOOKMARKS " );
}

void BookmarkManager::saveBookmarks()
{
    assert( m_xbeldoc );

    QFile file( m_bookmarkFile );
    if( !file.open( IO_WriteOnly ) ) 
        return;

    QTextStream stream( &file );
    stream.setEncoding( QTextStream::UnicodeUTF8 );
    m_xbeldoc->save( stream );
}

void BookmarkManager::createBookmarksMenu()
{
    XMLElement *docElem = m_xbeldoc->firstChild();
    assert( docElem );
    assert( docElem->tagName() == "xbel" );
 
    Bookmark bm( docElem );
    if( !bm.isFolder() ) 
        return;

    if ( !m_bookmarkMenu )
    {
        m_bookmarkMenu = new BookmarkMenu( bm.toFolder() );
        connect( m_bookmarkMenu, SIGNAL( activatedBookmark( const QString& ) ),
                 this, SLOT( bookmarkSelected( const QString& ) ) );
    }
    else
        m_bookmarkMenu->refill( bm.toFolder() );
}

#include "bookmarks.moc"

#endif // ENABLE_BOOKMARKS

/* vim: et sw=4
 */

