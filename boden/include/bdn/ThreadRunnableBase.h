#ifndef BDN_ThreadRunnableBase_H_
#define BDN_ThreadRunnableBase_H_

#include <bdn/IThreadRunnable.h>

namespace bdn
{
    

/** Provides a partial default implementation for IThreadRunnable.
 
    You can derive your class from ThreadRunnableBase and then you only
    need to override the run() method. Note that you run() implementation
    should regularly call shouldStop() and abort when that returns true.
 
 */
class ThreadRunnableBase : public Base, BDN_IMPLEMENTS IThreadRunnable
{
public:
    
    /** Derived classes should override this. The run method should perform
        the actual work of the thread.
     
        The run() implementation should regularly call shouldStop() and if that
        returns true then run() should abort its work and return as quickly
        as possible.
     */
    virtual void run() override=0;

    
    /** Returns true if the thread should stop (i.e. if signalStop() has been called).*/
    bool shouldStop() const
    {
        return _shouldStop;
    }

    void signalStop() override
    {
        _shouldStop = true;
    }
    
private:
    // note that we assume that a simple boolean is ok to use in a multithreaded context.
    // Since the bool is only checked for 0 and !=0 there are no possible race conditions
    // and partial updates cannot occur. So no need to use a (potentially slower) std::atomic here.
    volatile bool _shouldStop = false;
};
    
    
    
}

#endif

