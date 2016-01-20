#ifndef BDN_Mutex_H_
#define BDN_Mutex_H_

#include <mutex>

namespace bdn
{


/** A recursive Mutex. Only one thread at a time can hold a lock on the mutex object.
	If another thread tries to lock it at the same time then it block and wait until the
	thread that curently has it locked unlocks it.

	The mutex is recursive. That means that the same thread can hold multiple locks on the mutex.
	*/
class Mutex : public Base
{
public:
	Mutex()
	{
	}

	void lock()
	{
		_mutex.lock();
	}

	void unlock()
	{
		_mutex.unlock();
	}

protected:
	// mutex objects cannot be copied.
	Mutex(const Mutex&) = delete;

	std::recursive_mutex _mutex;
};

}


#endif
