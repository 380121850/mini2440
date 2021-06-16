#ifndef __KPLAINTEXT_PART_H
#define __KPLAINTEXT_PART_H

#include <config.h>

#ifdef KONQ_EMBEDDED
#ifndef ENABLE_READWRITE
#error The text editor requires --enable-readwrite
#endif
#endif

#include <kparts/part.h>
#include <kparts/factory.h>

class QWidget;
class QPainter;
class KURL;
class QMultiLineEdit;


class KPlainTextPart : public KParts::ReadWritePart
{
    Q_OBJECT

public:
    KPlainTextPart( QWidget *parentWidget, const char *widgetName,
                    QObject *parent, const char *name );

    virtual ~KPlainTextPart();

    virtual void setReadWrite( bool rw );
    virtual void setModified( bool modified );

protected:
    virtual bool openFile();
    virtual bool saveFile();
    virtual void setupActions();
    virtual void setCaption();
    const QString suggestedName();

protected slots:
    void fileOpen();
    void fileSaveAs();

private:
    QMultiLineEdit *m_widget;
};


class KInstance;
class KAboutData;

class KPlainTextPartFactory : public KParts::Factory
{
    Q_OBJECT

    friend class KPlainTextPart;
public:
    KPlainTextPartFactory();
    virtual ~KPlainTextPartFactory();
    virtual KParts::Part* createPartObject( QWidget *parentWidget, const char *widgetName,
                                            QObject *parent, const char *name,
                                            const char *classname, const QStringList &args );
    static KInstance* instance();
 
protected:
    static KInstance* s_instance;
    static KAboutData* s_about;
};

#endif // __KPLAINTEXT_PART_H
