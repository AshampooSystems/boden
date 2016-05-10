#ifndef BDN_IThreadRunnable_H_
#define BDN_IThreadRunnable_H_


namespace bdn
{
    
    
/** An interface for a function object that can be run inside a thread
    (see #Thread).
 
    See ThreadRunnableBase for a helper base class that provides a partial default implementation.
 */
class IThreadRunnable : BDN_IMPLEMENTS IBase
{
public:
    
    /** This function is called in the new thread. It should perform
        the actual work that the thread is meant to do.
     
        The implementation of run should check regularly if the thread should stop
        (see signalStop() ). If a stop has been signalled then run should return as soon
        as possible.
     
        It is fine for the run() function to throw an exception. The exception
        will be intercepted by the Thread object and can be handled in another thread
        when Thread::join() or Thread::stop() are called.
     */
    virtual void run()=0;
    
    
    /** Signals the run() function that it should stop.
        This function should set some kind of internal flag that is checked regularly in run.
     
        signalStop does NOT wait for run to actually stop. It only sets the internal flag and
        returns immediately.
     
        This function must be thread-safe (i.e. any thread must be allowed to call this).
     
        */
    virtual void signalStop()=0;
    
    
    
};
    
}

#endif
