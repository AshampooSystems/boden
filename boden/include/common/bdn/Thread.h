#ifndef BDN_Thread_H_
#define BDN_Thread_H_

#include <future>
#include <thread>
#include <utility>

#include <bdn/IThreadRunnable.h>
#include <bdn/ThreadRunnableBase.h>
#include <bdn/ThreadDetachedError.h>

namespace bdn
{
    
    
/** Thread objects are used to manage the execution of a new thread.
 
    Quick usage via static method
    =============================
 
    The static method Thread::exec() can be used to quickly run a function in another thread.
 
 
    Using a Thread object
    =====================
 
    If you need more control than Thread::exec() provides (for example, if you want the ability to
    abort the thread) then you should create an object that implements IThreadRunnable and pass that object
    to the #Thread constructor.
    
    ThreadRunnableBase is a good starting point for implementing IThreadRunnable.
    
    See IThreadRunnable for more information on how exactly to implement it.
    
    Starting the thread
    -------------------
 
    The thread starts automatically immediately when the Thread object is constructed with the runnable
    object. It is not necessary to start it manually.
 
 
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
 
    When the implementation of IThreadRunnable::run() throws an exception then that exception is stored.
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

    
    
    /** Constructs a dummy Thread object that is not actually connected to a real
        thread. It behaves like an object of a thread that has already finished.*/
    Thread() noexcept;
    
    
    /** Creates a thread which calls the IThreadRunnable::run() method of the specified runnable object.
     
        The thread will keep a reference to the runnable object. The object is released when the thread ends.
     
        If you prefer to pass a function pointer or function object directly then please look
        at the static function Thread::exec() instead.
     */
    Thread( IThreadRunnable* pRunnable );
    
    
    Thread(const Thread&) = delete;
    
    
    /** Destructs the thread object. If the thread was not detached then
        then it is stopped and the destructor waits for it to end (like calling stop() ). */
    ~Thread() noexcept;
    
    
    /** Returns the thread's id.
    
        The Id also remains available after the thread ended or join(), stop() or detach were called.
     
        If the Thread object was default-constructed (without parameters) then a dummy id that equals a default-constructed Id-object
        is returned.
     */
    Id getId() const
    {
        return _threadId;
    }
    
    
    /** Returns the native operating system handle of the thread. This is implementation specific.*/
    Handle getHandle()
    {
        return _thread.native_handle();
    }
    
    
    /** Detaches the thread from the Thread object.
     
        The thread will continue to run independently of the Thread object.
        
        After detaching the join() and stop() methods will throw an exception.
     
        This function can be called from any thread.
     
        It is no problem to call detach multiple times. The latter calls have no effect.
     
     */
    void detach() noexcept;
    
    
    enum ExceptionForwarding
    {
        /** If the thread aborted with an exception then re-throw it.*/
        ExceptionThrow,
        
        /** Ignore it if the thread aborted with an exception. Do not re-throw it.*/
        ExceptionIgnore,
    };
    
    
    /** Waits for the thread to end.
     
        If the thread has already ended then join returns immediately.
     
        If detach() has been called on the Thread object then join() always throws a ThreadDetachedError.
     
        If the thread aborted with an exception then exceptionForwarding controls how join will handle this.
        A value of Thread::ExceptionThrow will cause join to re-throw that exception. A value
        of Thread::ExceptionIgnore will cause join to ignore such an exception.
     
        This function can be called from any thread.
     
        It is no problem to call join multiple times. The latter calls have no effect.
     */
    void join( ExceptionForwarding exceptionForwarding );
    
    
    /** Signals the thread to stop/abort and waits for it to end.

        If the thread has already ended then stop returns immediately.
     
        If detach() has been called on the Thread object then stop() always throws a ThreadDetachedError.
     
        If the thread aborted with an exception then exceptionForwarding controls how join will handle this.
        A value of Thread::ExceptionThrow will cause join to re-throw that exception. A value
        of Thread::ExceptionIgnore will cause join to ignore such an exception.

        Note that it is the responsibility of the IThreadRunnable::run() method implementation
        to regularly check the stop condition and end when it is true. If it does
        not do that then stop can potentially block for a long time.
        See IThreadRunnable::run() for more information.
     
        This function can be called from any thread.
     
        It is no problem to call stop multiple times. The latter calls have no effect.
     
        */
    void stop( ExceptionForwarding exceptionForwarding );
    
    
    /** Signals the thread to stop and end as soon as possible. Note that it is the responsibility
        of the IThreadRunnable implementation to react to the stop signal and abort its work.
        See IThreadRunnable::run() for more information.
     
        This function can be called from any thread.
     
        If stop has already been signalled, or if the thread has already ended then
        signalStop has no effect.
        
        If detach() has been called on the Thread object then stop() always throws a ThreadDetachedError.
     */
    void signalStop();
     
    
    
    
    
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
     
        IMPORTANT: This function is only meant to be used for threads that do not need to support premature stopping or aborting.
        For longer running threads one should instead implement the IThreadRunnable interface and use a #Thread object instead.
     
        Thread::exec returns a future object that can be used to access the result of the function and/or
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
        std::function<typename std::result_of<FuncType(Args...)>::type ()> boundFunc = std::bind(std::forward<FuncType>(func), std::forward(args)...);
    
        class ExecThreadRunnable : public ThreadRunnableBase
        {
        public:
            ExecThreadRunnable( std::packaged_task< typename std::result_of<FuncType(Args...)>::type () >* pTask )
            {
                _pTask = pTask;
            }
            
            ~ExecThreadRunnable()
            {
                delete _pTask;
            }
            
            void run()
            {
                (*_pTask)();
            }
            
        protected:
            std::packaged_task< typename std::result_of<FuncType(Args...)>::type () >* _pTask;
        };
        
        
        
        std::packaged_task< typename std::result_of<FuncType(Args...)>::type () >* pTask
            = new std::packaged_task< typename std::result_of<FuncType(Args...)>::type () >(
                boundFunc );
        
        Thread execThread( newObj<ExecThreadRunnable>(pTask) );
        
        execThread.detach();
        
        // note that pTask is guaranteed to still be exist here. ExecThread deletes the object
        // in its destructor, but we still hold a reference to ExecThread at this point. So the
        // object cannot have been deleted yet, even if the thread already finished.
        return pTask->get_future();
    }
    

    

	/** For internal use only - do not call. Sets the Id of the main thread.*/
	static void _setMainId( const Id& id);
    
    
private:
    

    struct ThreadData : public Base
    {
        Mutex               runnableMutex;
        P<IThreadRunnable>  pRunnable;
        std::exception_ptr  threadException;
    };
    
    
    static void run( P<ThreadData> pThreadData );
    
	static Id& getMainIdRef()
	{
		static Id mainId;

		return mainId;
	}
    
    std::future<void>   _future;
    
    P<ThreadData>       _pThreadData;
    std::thread         _thread;
    Id                  _threadId;
    
    bool                _detached = false;
};
	

}

#endif