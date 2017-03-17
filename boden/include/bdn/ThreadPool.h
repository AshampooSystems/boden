#ifndef BDN_ThreadPool_H_
#define BDN_ThreadPool_H_


#if BDN_HAVE_THREADS

#include <bdn/Thread.h>
#include <bdn/ThreadRunnableBase.h>

namespace bdn
{
    
    
class ThreadPool : public Base
{
public:
    ThreadPool(int minThreadCount, int maxThreadCount)
        : _minThreadCount(minThreadCount)
        , _maxThreadCount(maxThreadCount)
    {
    }

    ~ThreadPool()
    {
        XXX stop busy and idle runners?
    }

    void schedule(IThreadRunnable* pRunnable)
    {
        MutexLock lock(_mutex);

        if(_idleRunners.empty())
        {
            // we have no idle runner waiting.

            if(_busyRunners.size()>=_maxThreadCount)
            {
                // we cannot start a new thread. Add the job to the queue
                _queuedJobs.push_back(pRunnable);
            }
            else
            {
                // start another thread.
                P<PoolRunner> pRunner = newObj<PoolRunner>(this);

                pRunner->setJob(pRunnable);

                _busyRunners.add( pRunner );

                try
                {
                    P<Thread> pThread = newObj<Thread>(pRunner);
                    pThread->detach();
                }
                catch(...)
                {
                    // if there is an error starting the thread then we remove the runner again.
                    _busyRunners.remove(pRunner);
                }
            }
        }
        else
        {
            // we have an idle runner waiting. Give it a new job
            P<PoolRunner> pRunner = _idleRunners.front();
            _idleRunners.pop_front();

            _busyRunners.add( pRunner );

            pRunner->startJob(pRunnable);
        }
    }

protected:
    class PoolRunner : public Base, BDN_IMPLEMENTS IThreadRunnable
    {
    public:
        PoolRunner()
        {
        }

        void signalStop() override
        {
            // this is called when the thread pool shuts down.

            {
                MutexLock lock(_mutex);

                _shouldStop = true;

                if(_pJob!=nullptr)
                    _pJob->signalStop();
            }

            // make sure that we wake up if we are waiting for a runnable
            _wakeSignal.set();
        }

        void setJob(IThreadRunnable* pJob)
        {
            if(_pJob!=null)
                throw ProgrammingError("ThreadPool::PoolRunnable::startRunnable was called while the thread was still busy.");

            _pJob = pJob;

            _wakeSignal.set();
        }

        void run() override
        {
            while(true)
            {
                _wakeSignal.wait();
                _wakeSignal.reset();

                {
                    MutexLock lock(_mutex);
                    if(_shouldStop)
                        break;

                    if(_pJob)
                }

                try
                {
                    _pJob->run();
                }
                catch(...)
                {
                    // we treat this just like a top-level exception that happens
                    // in a normal Thread. So we cann unhandledException.
                    if( ! bdn::unhandledException(true) )
                        std::terminate();

                    // ignore exception and continue.
                }

                try
                {
                    _pJob = nullptr;
                }
                catch(...)
                {
                    // exception in runnable destructor.

                    if( ! bdn::unhandledException(true) )
                        std::terminate();

                    // continue and do not delete the runnable object
                    _pJob.detachPtr();
                }
                
                if( ! _pPool->jobFinished(this) )
                {
                    // the pool wants us to end
                    break;
                }
            }
        }

    protected:
        Mutex           _mutex;
        Signal          _wakeSignal;
        volatile bool   _shouldStop = false;

        P<IThreadRunnable> _pJob;
    };
    friend class PoolRunner;


    bool jobFinished( PoolRunner* pRunner )
    {
        MutexLock lock(_mutex);

        if(!_queuedJobs.empty())
        {
            P<IThreadRunnable> pJob = _queuedJobs.front();
            _queuedJobs.pop_front();

            // give the runner a new job right away
            pRunner->startJob(pJob);

            // Note that the runner remains in _busyRunners

            // runner should continue
            return true;
        }
        else
        {
            // we do not currently have anything queued.

            _busyRunners.remove( pRunner );

            if(_busyRunners.size() >= _minThreadCount)
            {
                // we have more threads than necessary. Let this one die.
                return false;
            }
            else
            {
                // add the runner to the idle list and let it go to sleep
                _idleRunners.push_back(pRunner);

                // runner should not end, but wait for the next job
                return true;
            }
        }
    }

    Mutex                      _mutex;

    std::list< P<PoolRunner> > _idleRunners;
    std::set< P<PoolRunner> >  _busyRunners;

    int _minThreadCount;
    int _maxThreadCount;
};
	

}

#endif // BDN_HAVE_THREADS

#endif