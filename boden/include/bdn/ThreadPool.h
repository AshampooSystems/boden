#ifndef BDN_ThreadPool_H_
#define BDN_ThreadPool_H_


#if BDN_HAVE_THREADS

#include <bdn/Thread.h>
#include <bdn/ThreadRunnableBase.h>
#include <bdn/Signal.h>

#include <bdn/List.h>
#include <set>

namespace bdn
{
    
    
/** Implements a pool of worker threads that can be used to execute jobs.

    A thread pool is often used to minimize the amount of threads that are created and destroyed
    for small tasks (since creating a new thread for each short job can be quite expensive).
*/
class ThreadPool : public Base
{
public:
    
    /** Constructor.
    
        \param minThreadCount minimum number of threads to keep running at all times (even
            if there are no jobs waiting).
        \param maxThreadCount maximum number of threads to have running simultaneously.
            If the pool has more jobs than this then they will be added to the waiting queue.
            If maxThreadCount is bigger than minThreadCount then this can be thought of as extra
            "burst" capacity. More threads than the minimum can be started when there is a lot to do,
            but when this extra capacity is not needed anymore then then these extra threads can be
            destroyed again.
    */
    ThreadPool(int minThreadCount, int maxThreadCount);
    ~ThreadPool();


    /** Adds a job for the thread pool to execute. Note that if the pool currently
        has no free capacity then the job will not start right away. Instead it 
        will be added to a waiting queue and start later.*/
    void addJob(IThreadRunnable* pRunnable);
    

    /** Returns the number of threads that are currently busy. Note that this is number
        can change at any time when jobs finish or get started, so it is only fully
        reliable in rare cases (possibly during testing).*/
    int getBusyThreadCount() const;


    /** Returns the number of idle threads (threads that are waiting for new jobs).
        Note that this is number can change at any time when jobs finish or are added,
        so it is only fully reliable in rare cases (possibly during testing).*/
    int getIdleThreadCount() const;


private:
    class PoolRunner : public Base, BDN_IMPLEMENTS IThreadRunnable
    {
    public:
        PoolRunner(ThreadPool* pPool)
        {
            _pPoolWeak = pPool;
        }

        void signalStop() override;
        void startJob(IThreadRunnable* pJob);

        void run() override;

    protected:
        WeakP<ThreadPool> _pPoolWeak;

        Mutex           _mutex;
        Signal          _wakeSignal;
        volatile bool   _shouldStop = false;

        P<IThreadRunnable> _pJob;
    };
    friend class PoolRunner;


    bool runnerFinishedJob( PoolRunner* pRunner );

    mutable Mutex              _mutex;

    List< P<PoolRunner> >       _idleRunners;
    std::set< P<PoolRunner> >   _busyRunners;

    List< P<IThreadRunnable> >  _queuedJobs;

    int                        _minThreadCount;
    int                        _maxThreadCount;
};
	

}

#endif // BDN_HAVE_THREADS

#endif

