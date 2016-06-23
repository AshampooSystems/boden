#ifndef BDN_MutexUnlock_H_
#define BDN_MutexUnlock_H_

#include <bdn/Mutex.h>

namespace bdn
{


/** Unlocks a mutex when the object is constructed and re-locks it when the object is destructed.

	This can be used to temporarily unlock a mutex.
	
	Example:
	\code

	void myFunction()
	{
		_myMutex.lock();

		{
			MutexUnlock unlock(_myMutex);

			// mutex is now unlocked

			... do stuff while unlocked
		}

		// mutex is locked again here, since the MutexUnlock object was destroyed.

		... do more stuff while locked

		_myMutex.unlock();
	}
	
	\endcode
	*/
class MutexUnlock
{
public:
	MutexUnlock(Mutex& mutex)
		: MutexUnlock(&mutex)
	{
	}

	MutexUnlock(Mutex* pMutex)
	{
		_pMutex = pMutex;

		_pMutex->unlock();
	}

	~MutexUnlock()
	{
		_pMutex->lock();
	}

protected:
	P<Mutex> _pMutex;
};


}


#endif
