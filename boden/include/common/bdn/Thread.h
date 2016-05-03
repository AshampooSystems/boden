#ifndef BDN_Thread_H_
#define BDN_Thread_H_

#include <thread>

namespace bdn
{

class Thread : public Base
{
public:

	/** A thread Id. Such Id objects can be used to identify threads.
		Id objects can be compared to each other with the normal comparison
		operators ==, !=, <, >, etc.

		The Id object can also be serialized into std ostream objects using the << operator.
		*/
	typedef std::thread::id Id;



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



	/** For internal use only - do not call. Sets the Id of the main thread.*/
	static void _setMainId( const Id& id);

private:
	static Id& getMainIdRef()
	{
		static Id mainId;

		return mainId;
	}
};
	

}

#endif