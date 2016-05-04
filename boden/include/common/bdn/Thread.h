#ifndef BDN_Thread_H_
#define BDN_Thread_H_

#include <thread>
#include <future>

namespace bdn
{
    
    
/** Thread objects are used to manage the execution of a new thread.
 
    Quick usage via static method
    =============================
 
    The static method Thread::exec() can be used to quickly run a function in another thread.
 
    If more control is desired then one should derive a class from Thread instead.
 
 
    Using via a derived class
    =========================
 
    run method
    ----------
 
    To implement a thread you should derive your own class from Thread
    and then override the run() method.
 
    The run() method implementation should regularly call shouldStop() to check 
    if it should abort execution and exit.
 
 
    Starting the thread
    -------------------
 
    The actual thread is not created automatically. It must be started manually
    by calling start().
 
 
    Stopping the thread
    -------------------
 
    Threads can be stopped/aborted with stop() or signalStop().
 

    Waiting for a thread to finish
    ------------------------------
 
    One can wait for a thread to finish with join().
 

    Thread object destruction
    -------------------------
 
    When the Thread object is destructed then the thread is stopped by default
    if it is still running (like calling stop() ).
 
    If you want the thread to continue running when the Thread object is destructed
    then you need to call detach() before it is destroyed.
 
 
    Exceptions in threads
    ---------------------
 
    When the implementation of run() throws an exception then that exception is stored.
    join() and stop() can re-throw this exception, thus allowing their caller to handle
    them from the managing thread.
 
 
    Static utility functions
    ========================
 
    Thread also offers some static utility functions like isCurrentMain(), getCurrentId(),
    sleepSeconds(), sleepMillis(), yield(), etc.
 
 */
class Thread : public Base
{
public:

	/** The type of a thread id object. Such Id objects can be used to identify threads.
		Id objects can be compared to each other with the normal comparison
		operators ==, !=, <, >, etc.

		The Id object can also be serialized into std ostream objects using the << operator.
		*/
	typedef std::thread::id Id;

    /** The type of the native operating system handle to the thread. This is implementation-specific.*/
    typedef std::thread::native_handle_type Handle;

    
    
    Thread() noexcept
    {
    }
    
    Thread(const Thread&) = delete;
    
    
    /** Destructs the thread object. If the thread is still running then it is stopped and
        the destructor waits for it to end (like calling stop() ).*/
    ~Thread() noexcept;
    
    
    /** Returns the thread's id.
     
        If the Thread object is invalid (see isValid()) then a default-constructed Id object
        is returned.
     */
    Id getId() const
    {
        return _thread.get_id();
    }
    
    
    /** Returns the native operating system handle of the thread. This is implementation specific.*/
    Handle getHandle()
    {
        return _thread.native_handle();
    }
    
    
    /** Causes the thread to run in detached mode.
     
        When detach is called, the running background thread will take ownership
        of the Thread object and keep it alive while the thread is running. That ensures
        that the run() implementation can still rely on the Thread object remaining
        valid.
     
        After detach has been called, it is fine for the caller to release its
        last reference to the Thread object. Since the background thread keeps the
        Thread object alive on its own, the Thread object will NOT be destructed, even if
        there are no more outside references to it.
        
        The thread will simply keep running, until its run() method finishes. Once run()
        has returned the Thread object will release its self-reference and at that point
        the Thread object will be destroyed (assuming that there are no more outside references
        to it).
     */
    void detach();
    
    
    /** Waits for the thread to end.
     
        The Thread object becomes invalid afterwards (see isValid()).
     
        join has no effect if the Thread object is already invalid.*/
    void join();
    
    
    /** Signals the thread to stop and waits for it to end.
     
        Note that it is the responsibility of the run() method implementation
        to regularly check the stop condition and end when it is true. If it does
        not do that then stop can potentially block for a long time.
        See run() for more information.
     
        */
    void stop();
    
    
    /** Signals the thread to stop.
     
        The default implementation simply sets a flag that can be checked with shouldStop().
        A derived class can override this to implement a custom way to communicate the
        stop command to the run() implementation.*/
    virtual void signalStop();
     
     
    
    
    
    
    /** Lets the current thread sleep for the specified number of seconds.
        The seconds parameter is a double, so you can also pass fractional values here.
     
        If \c seconds is negative or 0 then the call is equivalent to yield().
     
        \code
     
        Thread::sleepSeconds( 1.5 );	// sleep for 1500 milliseconds.
     
        \endcode
     */
    static void sleepSeconds(double seconds);
    
    
    /** Lets the current thread sleep for the specified number of milliseconds.
     
     If \c millis is negative or 0 then the call is equivalent to yield().
     */
    static void sleepMillis(int64_t millis );
    
    
    /** Causes the thread to yield the remainder of its current execution time slice
        to another thread. This gives other threads the opportunity to execute.
     
        Sometimes when a thread runs in a tight loop and does not call certain operating
        system functions it can prevent other threads from running. This is where yield
        is useful: it gives other threads an opportunity to get their share of the system's
        processing power.*/
    static void yield() noexcept;
    
    
    /** Static function that returns the id of the current thread.*/
    static Thread::Id getCurrentId();
    
    
    /** Static function that returns the id of the main thread.*/
    static Thread::Id getMainId();
    
    
    /** Static function that returns true if the current thread is the main thread.
     
        In an application with a graphical user interface the main thread is the thread
        that runs the user interface and event loop.
     
        In a commandline app the main thread is simply the first thread of the program
        (the one that the main function runs in).
     
     */
    static bool isCurrentMain();
    

    
    /** A static convenience function that executes the specified function with the specified arguments in a new thread.
     
        Returns a future object that can be used to access the result of the function and/or
        to wait for it to finish.
     
        If you are not interested in the result of the function it is not necessary to store the returned
        future object. Even if the future object is destroyed the thread still continues executing in the background.
     
        Thread::exec is somewhat similar to std::async. However, with std::async one has to always store the returned future object,
        because its destructor will wait for the thread to finish. Thread::exec behaves differently, as described above.
     
        Usage example
        -------------
     
        \code
     
        int myFunc(String a, double b)
        {
            ...
        }
     
        // --- Variant 1: access the result ---
     
        std::future<int> result = Thread::exec( myFunc, "some string parameter", 42.42 );
     
        // Optional: wait for the thread to finish with a 1000 ms timeout
        result.wait_for( std::chrono::milliseconds( 1000 ) );
     
        // Now we wait for the thread to fully finish and get the result.
        // If myFunc threw an exception then get will re-throw it here.
        int value = result.get();
     
     
        // --- Variant 2: just execute, without accessing the result --- 
     
        // we simply don't store the result object
        Thread::exec( myFunc, "some string parameter", 42.42 );
     
        // the thread will now execute on its own in the background. We do not wait for it.
     
        \endcode
     
     */
    template <class FuncType, class... Args>
    static std::future<typename std::result_of<FuncType(Args...)>::type> exec(FuncType&& func, Args&&... args)
    {
        class ExecThread : public Thread
        {
        public:
            ExecThread( std::packaged_task< typename std::result_of<FuncType(Args...)>::type >* pTask )
            {
                _pTask = pTask;
            }
            
            ~ExecThread()
            {
                delete _pTask;
            }
            
            void run()
            {
                _pTask->make_ready_at_thread_exit();
            }
            
        protected:
            std::packaged_task< typename std::result_of<FuncType(Args...)>::type >* _pTask;
        };
        
        std::packaged_task< typename std::result_of<FuncType(Args...)>::type >* pTask
            = new std::packaged_task< typename std::result_of<FuncType(Args...)>::type >(
                std::bind(func, args...) );
        
        P<ExecThread> pThread = newObj<ExecThread>(pTask);
        
        pThread->start();
        
        pThread->detach();
        
        // note that pTask is guaranteed to still be exist here. ExecThread deletes the object
        // in its destructor, but we still hold a reference to ExecThread at this point. So the
        // object cannot have been deleted yet, even if the thread already finished.
        return pTask->get_future();
    }
    

    

	/** For internal use only - do not call. Sets the Id of the main thread.*/
	static void _setMainId( const Id& id);
    
protected:
    
    /** This must be overridden in a derived class to implement the actual
        work the thread is supposed to perform.
     
        The run implementation should take care to regularly check shouldStop()
        and aborts its work when it returns true. Otherwise the stop() method
        and potentially the Thread destructor can block for a long time, waiting
        for the thread to exit.
     
        Alternatively one can also override signalStop() and implement a custom
        way to communicate the stop signal to the run implementation.
     
        It is ok for run() to throw an exception in case of errors. The exception
        will automatically be stored and can be accessed via join() or stop().
     */
    virtual void run()=0;
    
    
private:
    
    void runWrapper();
    
	static Id& getMainIdRef()
	{
		static Id mainId;

		return mainId;
	}
    
    std::thread _thread;
};
	

}

#endif