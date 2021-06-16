#ifndef __dcopobject_h__
#define __dcopobject_h__

#include <qcstring.h>
#include <qlist.h>
#include <qobject.h>
#include <qvaluelist.h>

// Makros for DCOP interfaces

typedef QValueList<QCString> QCStringList;

#define K_DCOP \
public:        \
  virtual bool process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData); \
  QCStringList functions(); \
  QCStringList interfaces(); \
private:

#define k_dcop public
#define ASYNC void


class DCOPObject : public QObject
{
    Q_OBJECT
public:
    DCOPObject();
    DCOPObject( const QCString &name );
    virtual ~DCOPObject();

    virtual bool process( const QCString &fun, const QByteArray &data,
                          QCString &replyType, QByteArray &replyData );

    virtual QCStringList functions();
    virtual QCStringList interfaces();

    void setObjId( const QCString &name );
    QCString objId() const { return m_objId; }

private:
    QCString m_objId;
};

#endif
