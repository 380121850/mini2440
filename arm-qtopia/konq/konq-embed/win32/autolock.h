#ifndef __autolock_h__
#define __autolock_h__

class AutoLock
{
public:
    AutoLock( QMutex &mutex ) : m_mutex( mutex ) { m_mutex.lock(); }
    ~AutoLock() { m_mutex.unlock(); }

    QMutex *operator &() const { return &m_mutex; }

private:
    QMutex &m_mutex;
};

#endif
