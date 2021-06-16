/*  This file is part of the KDE project
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception this program may be linked with Qt non-commercial 
    edition, the resulting executable be distributed, without including the 
    source code for the Qt non-commercial edition in the source distribution.

*/

#include "scheduler.h"

#include "jobclasses.h"
#include "slavebase.h"
#include "slave.h"
#include "launcher.h"
#include "kprotocolmanager.h"

#include <dcopclient.h>
#include <dcopobject.h>
#include <kdebug.h>

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace KIO;

Scheduler::ProtocolInfo::ProtocolInfo()
{
    for(int i = 0; i < SAM_MAX_TASKS; i++) {
	m_slaves[i] = 0;
        m_slavehost[i] = "<unknown>";
	m_running[i] = false;
    }
    m_running_cnt = 0;
}
Scheduler::ProtocolInfo::~ProtocolInfo()
{
}

Scheduler::JobInfo::JobInfo(SimpleJob* job)
    : m_job( job ), m_scheduled( false )
{
}

Scheduler *Scheduler::s_self = 0;

Scheduler::Scheduler()
{
    connect( &m_timer, SIGNAL( timeout() ), this, SLOT( slotStep() ) );
}

Scheduler::~Scheduler()
{
}

bool Scheduler::doJob( SimpleJob *job )
{
    KURL url = job->url();

    if ( !SlaveBase::knownProtocol( url.protocol() ) )
        return false;

    ProtocolInfo *nfo = m_schedule[ url.protocol() ];

    if ( !nfo )
    {
        nfo = new ProtocolInfo;
        m_schedule.insert( url.protocol(), nfo );
    }

    //job already in list ?
    for(QListIterator <JobInfo>job_it(nfo->m_jobs); job_it.current(); ++job_it)
        if ( job_it.current()->m_job == job )
            return false;

    JobInfo* ji = new JobInfo( job );

    nfo->m_jobs.append( ji );

    m_timer.start( 0, true );

    return true;
}

void Scheduler::releaseJob( SimpleJob *job, bool _killSlave )
{
    kdDebug() << "releaseJob " << job << endl;

    // the job's url might have changed or something, so let's iterate
    // over all entries
    QDictIterator<ProtocolInfo> it( m_schedule );
    for (; it.current(); ++it )
    {
        kdDebug() << it.currentKey() << " : current " << it.current()->m_jobs.current() << endl;

        JobInfo* ji = it.current()->m_jobs.first();
        while ( ji ) {
	    if(ji->m_job == job) {
		int task = job->taskNumber();
		if(task == -1) {
		    qDebug( "sam: shouldn't have happend" );
		} else {
		    it.current()->m_running[task] = false;
		    it.current()->m_running_cnt--;
		}
                delete it.current()->m_jobs.take();
                ji = it.current()->m_jobs.current();
		if ( _killSlave )
		    killSlave( it.current(), task );
	    }
            else
                ji = it.current()->m_jobs.next();
	}
    }
    job->setTaskNumber( -1 );
    m_timer.start( 0, true );
}

void Scheduler::scheduleJob( SimpleJob* job )
{
    Scheduler::self()->_scheduleJob( job );
}

void Scheduler::_scheduleJob(SimpleJob *job)
{
    QDictIterator<ProtocolInfo> it( m_schedule );
    for (; it.current(); ++it )
    {
	for(QListIterator <JobInfo>job_it(it.current()->m_jobs); job_it.current(); ++job_it) {
	    if(job_it.current()->m_job == job) {
                job_it.current()->m_scheduled = true;
                break;
            }
        }
    }
    m_timer.start(0, true);
}

void Scheduler::slotStep()
{
    QDictIterator<ProtocolInfo> it( m_schedule );
    for (; it.current(); ++it )
        doStep( it.current() );

    it.toFirst();
    while ( it.current() )
    {
        if ( it.current()->m_jobs.count() == 0 && !it.current()->m_running_cnt )
            delete m_schedule.take( it.currentKey() );
        else
            ++it;
    }
}

//#define SAM_DEBUG
#ifdef SAM_DEBUG
void Scheduler::debugInfo( ProtocolInfo* nfo )
{
    qDebug( "joblist: " );
    int i =1;

    for(QListIterator <JobInfo>job_it(nfo->m_jobs); job_it.current(); ++job_it, ++i) {
        qDebug( "job %d, Sched: %d, taskNumber: %d, url: %s", i,
                job_it.current()->m_scheduled, job_it.current()->m_job->taskNumber(),
                job_it.current()->m_job->url().url().latin1() );
    }

    qDebug( "running count: %d", nfo->m_running_cnt );
    qDebug( "slaves: " );


    for ( int k = 0; k < SAM_MAX_TASKS; k++ )
        qDebug( "%3d: slave: %p, running: %d,  slavehost: %s",
                k, nfo->m_slaves[k], nfo->m_running[k],
                nfo->m_slavehost[k].data());

    qDebug( "\n\n" );

}
#endif

void Scheduler::doStep( ProtocolInfo *nfo )
{
#ifdef SAM_DEBUG
    debugInfo(nfo);
#endif

    if(nfo->m_running_cnt >= SAM_MAX_TASKS)
	return;

    if ( nfo->m_jobs.count() == 0 )
    {
        //work done.
        killSlave( nfo );
        return;
    }

    int task = -1;
    for(int i = 0; i < SAM_MAX_TASKS; i++) {
        if ( !nfo->m_running[i] && nfo->m_slaves[i] ) {
            for(QListIterator <JobInfo> it(nfo->m_jobs); it.current(); ++it) {
                SimpleJob *job = it.current()->m_job;
                if(job->taskNumber() != -1)
                    continue;

                if ( !strcmp(job->url().host().latin1(),nfo->m_slavehost[i] ) && it.current()->m_scheduled ) {
                    startJobSlaveTask(nfo, job, i );
                    break;
                }
            }

        }

    }

    for(QListIterator <JobInfo>it(nfo->m_jobs); it.current(); ++it) {
	SimpleJob *job = it.current()->m_job;
	if(job->taskNumber() != -1)
	    continue;

        QCString jobHost = job->url().host().latin1();
        int jobHostCount = 0;

	task = -1;
	for(int i = 0; i < SAM_MAX_TASKS; i++) {
            if (nfo->m_slaves[i] && nfo->m_slavehost[i] == jobHost )
                ++jobHostCount;

	    if(!nfo->m_slaves[i] || !nfo->m_running[i]) {
		task = i;
		break;
	    }
	}
	if(task == -1)
	    break;

        if ( jobHostCount > 2 )
            continue;

	if ( !nfo->m_slaves[task] ) {
	    kdDebug() << "creating slave for " << job->url().prettyURL() << endl;
            nfo->m_slaves[task] = Launcher::self()->createSlave( job->url().protocol() );
            nfo->m_slavehost[task] = job->url().host().latin1();
	}

        startJobSlaveTask( nfo, job, task );

    }
}

void Scheduler::startJobSlaveTask(ProtocolInfo* nfo, SimpleJob* job, int task )
{
    nfo->m_running_cnt++;
    nfo->m_running[task]=true;
    nfo->m_slavehost[task] = job->url().host().latin1();

    job->setTaskNumber(task);

    Slave *slave = nfo->m_slaves[task];
    MetaData configData = KProtocolManager::protocolConfig( job->url().protocol() );
    slave->setConfig( configData );
    job->start( slave );
}


void Scheduler::killSlave( ProtocolInfo *nfo, int task )
{
    for(int i = 0; i < SAM_MAX_TASKS; i++) {
	if ( nfo->m_slaves[i] &&
             ( i == task || task == -1 ) ) {
	    kdDebug() << "sam: killing " << i << endl;
	    nfo->m_slaves[i]->kill();
	    delete nfo->m_slaves[i];
	    nfo->m_slaves[i] = 0;
	    nfo->m_running[i]=false;
            nfo->m_slavehost[i] = "<unknown>";
	}
    }
    nfo->m_running_cnt = 0;
}

Scheduler *Scheduler::self()
{
    if ( !s_self )
        s_self = new Scheduler;
    return s_self;
}

#include "scheduler.moc"
