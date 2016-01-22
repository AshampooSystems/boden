#ifndef BDN_MutexLock_H_
#define BDN_MutexLock_H_

#include <bdn/Mutex.h>

namespace bdn
{


/** Represents a lock on a Mutex object.

	When the MutexLock object is constructed it locks the mutex. When the MutexLock
	object is destroyed it unlocks the mutex.

	Example:
	\code

	void myFunction()
	{
		if(x)
		{
			MutexLock lock(_myMutex);

			... do some stuff while mutex is locked.

			// lock is released here
		}
	}

	\endcode
	*/
class MutexLock
{
public:
	MutexLock(Mutex& mutex)
		: MutexLock(&mutex)
	{
	}

	MutexLock(Mutex* pMutex)
	{
		_pMutex = pMutex;

		_pMutex->lock();
	}

	~MutexLock()
	{
		_pMutex->unlock();
	}

protected:
	P<Mutex> _pMutex;
};


}


#endif
