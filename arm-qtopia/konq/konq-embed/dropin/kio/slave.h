#ifndef __kio_slave_h__
#define __kio_slave_h__

#include <kio/slaveinterface.h>
#include <kio/connection.h>

#include <unistd.h>

namespace KIO
{
    class Connection;

    class Slave : public KIO::SlaveInterface
    {
        Q_OBJECT
    public:
        Slave( Connection *conn, pid_t pid );
        virtual ~Slave();

        pid_t pid() const { return m_pid; }

        void kill();

        // convenience
        void send( int cmd, const QByteArray &data = QByteArray() )
            { connection()->send( cmd, data ); }

        void setConfig( const MetaData &config );

    private slots:
        void slotDispatch();

    private:
        pid_t m_pid;
    };

};

#endif
