#ifndef __khtml_ext_h__
#define __khtml_ext_h__

#include "khtml_part.h"
#include <kaction.h>

/**
 * This is the BrowserExtension for a @ref KHTMLPart document. Please see the KParts documentation for
 * more information about the BrowserExtension.
 */
class KHTMLPartBrowserExtension : public KParts::BrowserExtension
{
  Q_OBJECT
  friend class KHTMLPart;
  friend class KHTMLView;
public:
  KHTMLPartBrowserExtension( KHTMLPart *parent, const char *name = 0L );

  virtual int xOffset();
  virtual int yOffset();

  virtual void saveState( QDataStream &stream );
  virtual void restoreState( QDataStream &stream );

  void editableWidgetFocused( QWidget * ) {}
  void editableWidgetBlurred( QWidget * ) {}

  void setExtensionProxy( KParts::BrowserExtension * ) {}

    // HACK: put it here since this is a friend of KHTMLPart
    static void setOpenedByJS( KHTMLPart *part, bool _openedByJS )
	{ part->setOpenedByJS(_openedByJS); }

public slots:
  void copy() {}
  void reparseConfiguration();
  void print();

private:
  KHTMLPart *m_part;
};

class KHTMLPartBrowserHostExtension : public KParts::BrowserHostExtension
{
public:
  KHTMLPartBrowserHostExtension( KHTMLPart *part );

  virtual QStringList frameNames() const;

  virtual const QList<KParts::ReadOnlyPart> frames() const;

  virtual bool openURLInFrame( const KURL &url, const KParts::URLArgs &urlArgs );
private:
  KHTMLPart *m_part;
};

/**
 * @internal
 * INTERNAL class. *NOT* part of the public API.
 */
class KHTMLPopupGUIClient : public KXMLGUIClient
{
public:
  KHTMLPopupGUIClient( KHTMLPart *, const QString &, const KURL & )
        {}

  static void saveURL( QWidget *, const QString &, const KURL &,
                       const QString & = QString::null, long = 0 )
        {}
};

class KHTMLFontSizeAction : public KAction
{
public:
    KHTMLFontSizeAction( KHTMLPart *, bool, const QString &text, const QString &, const QObject *receiver, const char *slot, QObject *parent, const char *name )
        : KAction( text, 0, receiver, slot, parent, name ) {}
};

#endif
