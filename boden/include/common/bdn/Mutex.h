#ifndef BDN_Mutex_H_
#define BDN_Mutex_H_


// <mutex> header is not supported when we compile a .NET app.
#if BDN_PLATFORM_DOTNET
  #include <msclr/lock.h>

#else
  #include <mutex>

#endif

namespace bdn
{


/** A recursive Mutex. Only one thread at a time can hold a lock on the mutex object.
	If another thread tries to lock it at the same time then it block and wait until the
	thread that curently has it locked unlocks it.

	The mutex is recursive. That means that the same thread can hold multiple locks on the mutex.

	Each lock() call must be paired with exactly one unlock() call. It is recommended to use #MutexLock
	helper objects to guarantee that.

	There must not be any active locks on Mutex objects when they are destroyed! Otherwise undefined
	behaviour, crashes or assertions can occur.


	*/
class Mutex : public Base
{
public:
	Mutex()
	{
#if BDN_PLATFORM_DOTNET
		_lockRef = gcnew msclr::lock(gcnew System::Object, msclr::lock_later);
#endif
	}

	void lock()
	{
#if BDN_PLATFORM_DOTNET
		_lockRef->acquire();

#else
		_mutex.lock();
#endif
	}

	void unlock()
	{
#if BDN_PLATFORM_DOTNET
		_lockRef->release();

#else
		_mutex.unlock();

#endif
	}

protected:
	// mutex objects cannot be copied.
	Mutex(const Mutex&) = delete;

#if BDN_PLATFORM_DOTNET
	gcroot<msclr::lock ^>	_lockRef;

#else	
	std::recursive_mutex	_mutex;
#endif
};

}


#endif
