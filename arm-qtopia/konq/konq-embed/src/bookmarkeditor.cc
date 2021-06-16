#include <config.h>
#if defined(ENABLE_BOOKMARKS)
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '../../../../src/kdenox/konq-embed/src/bookmarkeditor.ui'
**
** Created: Mon Mar 11 10:36:22 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "bookmarkeditor.h"

#include <qframe.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"16 16 53 1",
". c None",
"k c #313031",
"m c #4a444a",
"# c #5a595a",
"h c #6a6d6a",
"i c #737173",
"l c #8b898b",
"q c #948983",
"d c #9c999c",
"c c #bdb6b4",
"p c #c5bab4",
"Y c #cda183",
"R c #cda58b",
"K c #cdae94",
"W c #cdb29c",
"A c #cdb6a4",
"j c #cdcecd",
"P c #d5ae94",
"X c #d5ae9c",
"F c #d5b6a4",
"V c #d5baac",
"w c #d5beb4",
"T c #d5cac5",
"U c #dec6bd",
"S c #ded2cd",
"b c #dedede",
"e c #e6dee6",
"o c #e6e2de",
"a c #e6e2e6",
"Q c #eecaac",
"O c #eeceb4",
"s c #eee2de",
"J c #f6d6c5",
"N c #f6dac5",
"I c #f6dacd",
"z c #f6decd",
"H c #f6e2d5",
"y c #f6e6d5",
"G c #f6e6de",
"L c #f6eade",
"g c #f6eef6",
"M c #ffe2d5",
"E c #ffe6de",
"D c #ffeade",
"v c #ffeae6",
"x c #ffeee6",
"C c #ffeeee",
"u c #fff2ee",
"B c #fff6f6",
"n c #fff6ff",
"t c #fffaf6",
"r c #fffaff",
"f c #ffffff",
"..#########.....",
"..#ababac#d#....",
"..#effffghfd#...",
"..#afffffijfdk..",
"..#effffflihmk..",
"..#afffffnopqk..",
"..#effffffrspk..",
"..#affffftuvwk..",
"..#efffftuxywk..",
"..#affrtxvyzAk..",
"..#effrBCDEzFk..",
"..#afruCGHIJKk..",
"..#efuCLMINOPk..",
"..#auxGHNJOQRk..",
"..#SSTUwVWXRYk..",
"..#kkkkkkkkkkk.."};

static const char* const image1_data[] = { 
"16 16 90 2",
"Qt c None",
".7 c #181810",
".w c #200c00",
".K c #201000",
".D c #291008",
".W c #312c20",
".R c #313031",
".X c #4a444a",
".2 c #523c31",
".Q c #5a4831",
".8 c #5a5052",
".# c #5a595a",
"#d c #73695a",
".V c #835539",
".P c #8b5531",
".0 c #94796a",
".9 c #9c9d94",
".C c #a44800",
".J c #a44808",
".s c #a44c00",
".1 c #a47152",
".6 c #a49583",
"#e c #a4958b",
".3 c #b4b2b4",
"#p c #bda17b",
"#b c #bda583",
".Y c #bdaa94",
"#f c #bdb6a4",
".U c #c57d5a",
"#x c #c5a17b",
"#j c #c5a583",
"#w c #c5aa83",
"#v c #c5aa8b",
".4 c #c5aa94",
"#u c #c5b294",
"#t c #c5b69c",
"#r c #c5beac",
".e c #cd7d41",
".O c #cd7d62",
".a c #cd8141",
"#s c #cdbaac",
"#. c #cdc2bd",
"#q c #cdc6bd",
".I c #d56529",
".H c #d57141",
".j c #d5ced5",
".u c #de9162",
".Z c #ded2c5",
"## c #ded6d5",
".d c #dedede",
"#c c #e6cab4",
"#n c #e6d2b4",
".m c #e6d6cd",
".h c #e6dee6",
".c c #e6e2e6",
".r c #ee7908",
".S c #eeb694",
"#o c #eeceac",
"#i c #eed2b4",
"#a c #eedabd",
"#h c #eedac5",
".p c #eedacd",
".t c #eedad5",
".L c #eee2de",
".o c #f67d08",
".A c #f69562",
".G c #f6b694",
".E c #f6ba9c",
".M c #f6d2b4",
"#g c #f6e2cd",
"#m c #f6e6d5",
"#l c #f6eade",
"#k c #f6eee6",
".x c #f6eef6",
".B c #ff8108",
".v c #ff8508",
".l c #ff8900",
".k c #ff8910",
".g c #ff9110",
".f c #ffc68b",
".n c #ffca8b",
".q c #ffca94",
".T c #ffcab4",
".b c #ffce8b",
".N c #ffd6bd",
".y c #ffdac5",
".z c #ffe2cd",
".F c #ffe6cd",
".5 c #fff6f6",
".i c #ffffff",
"QtQt.#.#.#.#.#.#.#.#.#.#.#.#.a.b",
"QtQt.#.c.d.c.d.c.d.c.d.cQt.e.f.g",
"QtQt.#.h.i.i.i.h.i.i.i.j.e.f.k.l",
"QtQt.#.c.i.i.i.c.i.i.m.e.n.k.l.o",
"QtQt.#.h.c.h.c.h.i.p.e.q.k.l.r.s",
"QtQt.#.c.i.i.i.c.t.u.f.v.l.r.s.w",
"QtQt.#.h.i.i.i.x.y.z.A.B.r.C.DQt",
"QtQt.#.c.d.c.d.E.F.G.H.I.J.KQtQt",
"QtQt.#.h.i.i.L.M.N.O.P.Q.D.RQtQt",
"QtQt.#.c.i.i.S.T.U.V.W.X.Y.RQtQt",
"QtQt.#.h.c.Z.0.1.2.#.3.3.4.RQtQt",
"QtQt.#.c.5.6.7.8.9#.###a#b.RQtQt",
"QtQt.#.h#c#d#e#f#g#h#a#i#j.RQtQt",
"QtQt.#.c#k#l#m#g#h#a#n#o#p.RQtQt",
"QtQt.##.#q#r#s#t#u#v#w#x#x.RQtQt",
"QtQt.#.R.R.R.R.R.R.R.R.R.R.RQtQt"};

static const char* const image2_data[] = { 
"16 16 5 1",
". c None",
"b c #410000",
"c c #830000",
"a c #c50000",
"# c #ff0000",
"................",
"................",
"............#...",
"....##.....#aa..",
"...#aa##..#aab..",
".....caa#aaab...",
"......caaaab....",
".......#aa#.....",
"......#aaa#.....",
".....#ab.ca#....",
".....#b...ca....",
"....#a.....ca...",
"....#b......a...",
"....#...........",
"................",
"................"};

static const char* const image3_data[] = { 
"16 16 54 1",
". c None",
"F c #000000",
"E c #000400",
"C c #081020",
"k c #081829",
"u c #081831",
"Z c #081c29",
"Y c #081c31",
"e c #082031",
"P c #082439",
"z c #0895bd",
"a c #101420",
"S c #102841",
"G c #104862",
"v c #104c6a",
"# c #104c73",
"B c #105073",
"b c #105573",
"t c #10557b",
"d c #10597b",
"K c #106183",
"X c #1085a4",
"M c #1099bd",
"y c #1099c5",
"r c #10a1cd",
"f c #18486a",
"o c #185073",
"j c #18557b",
"U c #1891b4",
"i c #1891bd",
"J c #1899bd",
"x c #18b2cd",
"q c #18b6d5",
"D c #203852",
"O c #2095b4",
"A c #20a5cd",
"R c #2999b4",
"I c #29bede",
"T c #29c6d5",
"n c #39d6e6",
"N c #41d6de",
"Q c #4adae6",
"s c #4ae2ee",
"W c #5aaec5",
"m c #5ac6de",
"p c #7bd6e6",
"c c #83a1bd",
"g c #83a1c5",
"V c #83c6e6",
"H c #a4e2f6",
"L c #cdeaf6",
"h c #def2ff",
"w c #e6f2ff",
"l c #ffffff",
"................",
"................",
".......#a.......",
"......bcde......",
".....fghijk.....",
"....fclmnidk....",
"...oglpqrsitu...",
"..vcwmxyzrnABC..",
".tDEFGHyIJKFFFF.",
".....tLMNOP.....",
".....blMQRS.....",
".....tLMTUP.....",
".....tVWIXF.....",
".....tkYZFF.....",
"................",
"................"};

static const char* const image4_data[] = { 
"16 16 56 1",
". c None",
"G c #000000",
"R c #000408",
"0 c #000810",
"X c #000c18",
"r c #001418",
"Z c #080c10",
"1 c #081018",
"i c #081420",
"w c #081820",
"F c #082841",
"C c #085573",
"U c #089dc5",
"d c #103452",
"x c #103852",
"H c #10385a",
"E c #10405a",
"D c #104462",
"# c #104c73",
"W c #10658b",
"Q c #106994",
"M c #1091bd",
"z c #1095bd",
"B c #1099bd",
"t c #1099c5",
"V c #10a1c5",
"Y c #10a5cd",
"o c #10aecd",
"c c #184062",
"a c #185073",
"P c #18a1c5",
"A c #18bad5",
"N c #18c2de",
"s c #204862",
"v c #2095b4",
"b c #294c6a",
"m c #2991b4",
"q c #2995b4",
"S c #317d9c",
"I c #3181a4",
"p c #41d2de",
"u c #4adeee",
"O c #4ae2ee",
"L c #5ac2de",
"k c #62c2de",
"h c #6aa5c5",
"l c #94ceee",
"e c #a4cae6",
"K c #acdeee",
"y c #c5deee",
"g c #d5e6f6",
"J c #e6f2ff",
"j c #e6f6ff",
"n c #eef2ff",
"f c #f6f6ff",
"T c #ffffff",
"................",
"................",
".....#a#bcd.....",
".....#efghi.....",
".....#jklmi.....",
".....#nopqr.....",
".....sjtuvw.....",
".x##sbyzABCDEFG.",
"..HIJKLMtNOPQR..",
"...xSTUtVOPWX...",
"....HITYOPQZ....",
".....xSJPW0.....",
"......HQQ1......",
".......xR.......",
"................",
"................"};


/* 
 *  Constructs a BookmarkEditorBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
BookmarkEditorBase::BookmarkEditorBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    QPixmap image2( ( const char** ) image2_data );
    QPixmap image3( ( const char** ) image3_data );
    QPixmap image4( ( const char** ) image4_data );
    if ( !name )
	setName( "BookmarkEditorBase" );
    resize( 283, 299 ); 
    setCaption( i18n( "Edit Bookmarks" ) );
    BookmarkEditorBaseLayout = new QGridLayout( this ); 
    BookmarkEditorBaseLayout->setSpacing( 6 );
    BookmarkEditorBaseLayout->setMargin( 11 );

    Frame4 = new QFrame( this, "Frame4" );
    Frame4->setFrameShape( QFrame::StyledPanel );
    Frame4->setFrameShadow( QFrame::Raised );
    Frame4Layout = new QHBoxLayout( Frame4 ); 
    Frame4Layout->setSpacing( 6 );
    Frame4Layout->setMargin( 6 );

    newBookmarkButton = new QToolButton( Frame4, "newBookmarkButton" );
    newBookmarkButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, newBookmarkButton->sizePolicy().hasHeightForWidth() ) );
    newBookmarkButton->setText( QString::null );
    newBookmarkButton->setPixmap( image0 );
    newBookmarkButton->setAutoRaise( TRUE );
    Frame4Layout->addWidget( newBookmarkButton );

    editBookmarkButton = new QToolButton( Frame4, "editBookmarkButton" );
    editBookmarkButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, editBookmarkButton->sizePolicy().hasHeightForWidth() ) );
    editBookmarkButton->setText( QString::null );
    editBookmarkButton->setPixmap( image1 );
    editBookmarkButton->setAutoRaise( TRUE );
    Frame4Layout->addWidget( editBookmarkButton );

    deleteBookmarkButton = new QToolButton( Frame4, "deleteBookmarkButton" );
    deleteBookmarkButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, deleteBookmarkButton->sizePolicy().hasHeightForWidth() ) );
    deleteBookmarkButton->setText( QString::null );
    deleteBookmarkButton->setPixmap( image2 );
    deleteBookmarkButton->setAutoRaise( TRUE );
    Frame4Layout->addWidget( deleteBookmarkButton );

    Line1 = new QFrame( Frame4, "Line1" );
    Line1->setFrameStyle( QFrame::VLine | QFrame::Sunken );
    Frame4Layout->addWidget( Line1 );

    moveUpButton = new QToolButton( Frame4, "moveUpButton" );
    moveUpButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, moveUpButton->sizePolicy().hasHeightForWidth() ) );
    moveUpButton->setText( QString::null );
    moveUpButton->setPixmap( image3 );
    moveUpButton->setAutoRaise( TRUE );
    Frame4Layout->addWidget( moveUpButton );

    moveDownButton = new QToolButton( Frame4, "moveDownButton" );
    moveDownButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, moveDownButton->sizePolicy().hasHeightForWidth() ) );
    moveDownButton->setText( QString::null );
    moveDownButton->setPixmap( image4 );
    moveDownButton->setAutoRaise( TRUE );
    Frame4Layout->addWidget( moveDownButton );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Frame4Layout->addItem( spacer );

    BookmarkEditorBaseLayout->addMultiCellWidget( Frame4, 0, 0, 0, 2 );

    okButton = new QPushButton( this, "okButton" );
    okButton->setText( i18n( "&OK" ) );

    BookmarkEditorBaseLayout->addWidget( okButton, 2, 1 );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setText( i18n( "&Cancel" ) );

    BookmarkEditorBaseLayout->addWidget( cancelButton, 2, 2 );

    view = new QListView( this, "view" );
    view->addColumn( i18n( "Title" ) );
    view->addColumn( i18n( "Location" ) );

    BookmarkEditorBaseLayout->addMultiCellWidget( view, 1, 1, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    BookmarkEditorBaseLayout->addItem( spacer_2, 2, 0 );

    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( view, SIGNAL( selectionChanged() ), this, SLOT( updateButtons() ) );
    connect( newBookmarkButton, SIGNAL( clicked() ), this, SLOT( newBookmark() ) );
    connect( editBookmarkButton, SIGNAL( clicked() ), this, SLOT( editBookmark() ) );
    connect( deleteBookmarkButton, SIGNAL( clicked() ), this, SLOT( deleteBookmark() ) );
    connect( moveUpButton, SIGNAL( clicked() ), this, SLOT( moveBookmarkUp() ) );
    connect( moveDownButton, SIGNAL( clicked() ), this, SLOT( moveBookmarkDown() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
BookmarkEditorBase::~BookmarkEditorBase()
{
    // no need to delete child widgets, Qt does it all for us
}

void BookmarkEditorBase::deleteBookmark()
{
    qWarning( "BookmarkEditorBase::deleteBookmark(): Not implemented yet!" );
}

void BookmarkEditorBase::editBookmark()
{
    qWarning( "BookmarkEditorBase::editBookmark(): Not implemented yet!" );
}

void BookmarkEditorBase::moveBookmarkDown()
{
    qWarning( "BookmarkEditorBase::moveBookmarkDown(): Not implemented yet!" );
}

void BookmarkEditorBase::moveBookmarkUp()
{
    qWarning( "BookmarkEditorBase::moveBookmarkUp(): Not implemented yet!" );
}

void BookmarkEditorBase::newBookmark()
{
    qWarning( "BookmarkEditorBase::newBookmark(): Not implemented yet!" );
}

void BookmarkEditorBase::updateButtons()
{
    qWarning( "BookmarkEditorBase::updateButtons(): Not implemented yet!" );
}

#include "bookmarkeditor.moc"
#endif
