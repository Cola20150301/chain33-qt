#include <QDir>
#include "clearthread.h"
#include "basefuntion.h"

ClearThread::ClearThread(QObject *parent):QThread(parent)
{
    m_quit = false;
}

void ClearThread::Resume()
{
    if(isRunning())
    {
        return;
    }
    m_quit = false;
    start();
}

ClearThread::~ClearThread()
{
    Stop();
}

void ClearThread::run()
{
    QString strUILog = GetDefaultDataDir() + "/logs";
    QDir dir;
    dir.setPath(strUILog);
    dir.setFilter(QDir::Files | QDir::Modified);
    dir.setSorting(QDir::Name);
    while(true)
    {
        if(m_quit)
        {
            return;
        }
        dir.refresh();
        QStringList names = dir.entryList();
        QStringList newnames;
        newnames=names.filter(".txt");
        if (newnames.length() > 1)
        {
            dir.remove(newnames[0]);
        }
        m_mutex.lock();
        m_cond.wait(&m_mutex, 1000*1000);
        m_mutex.unlock();
    }
}

void ClearThread::Stop()
{
    if(!isRunning())
    {
        return;
    }
    m_mutex.lock();
    m_quit = true;
    m_mutex.unlock();
    m_cond.wakeAll();
    wait();
}
