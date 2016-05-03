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