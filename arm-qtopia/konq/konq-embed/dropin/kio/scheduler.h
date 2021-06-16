#ifndef __kio_scheduler_h__
#define __kio_scheduler_h__

#include <qobject.h>
#include <qdict.h>
#include <qtimer.h>
#include <qlist.h>
#include <qcstring.h>

#define SAM_MAX_TASKS 6

namespace KIO
{
    class SimpleJob;
    class Slave;

    class Scheduler : public QObject
    {
        Q_OBJECT
    public:
        Scheduler();
        virtual ~Scheduler();

        bool doJob( SimpleJob *job );

        void releaseJob( SimpleJob *job, bool killSlave = false );

        // ### check if this is correct
        void _scheduleJob( SimpleJob * );
        static void scheduleJob(SimpleJob* );

        static Scheduler *self();

    private slots:
        void slotStep();

    private:
        struct JobInfo
        {
            JobInfo(SimpleJob* job);
            
            SimpleJob* m_job;
            // is this a job that has scheduleJob() been called on?
            bool m_scheduled;
        };
                
        struct ProtocolInfo
        {
            ProtocolInfo();
	    ~ProtocolInfo();

            QList<JobInfo> m_jobs;
            bool m_running[SAM_MAX_TASKS];
            Slave *m_slaves[SAM_MAX_TASKS];
            QCString m_slavehost[SAM_MAX_TASKS];
	    int m_running_cnt;
        };

        void doStep( ProtocolInfo *nfo );
        static void debugInfo(ProtocolInfo* info);

        void startJobSlaveTask(ProtocolInfo* nfo, SimpleJob* job, int task);

        void killSlave( ProtocolInfo *nfo, int task = -1 );

        QTimer m_timer;
        QDict<ProtocolInfo> m_schedule;
        static Scheduler *s_self;
    };

};

#endif
