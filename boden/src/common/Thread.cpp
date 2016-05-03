#include <bdn/init.h>
#include <bdn/Thread.h>

namespace bdn
{
	

void Thread::sleepSeconds(double seconds)
{
	sleepMillis( (int64_t) (seconds*1000) );
}

void Thread::sleepMillis( int64_t millis)
{
	if(millis<=0)
		yield();
	else		
		std::this_thread::sleep_for( std::chrono::milliseconds::duration( millis ) );
}

void Thread::yield() noexcept
{
	std::this_thread::yield();
}

Thread::Id Thread::getCurrentId()
{
	return std::this_thread::get_id();
}


Thread::Id Thread::getMainId()
{
	// The main ID is set only once and then never changed again.
	// So there is no need for any synchronization or mutexes.
	Id mainId = getMainIdRef();
	if(mainId==Id())
		throw ProgrammingError("Thread::getMainId called but main thread ID was not set yet.");

	return mainId;
}

void Thread::_setMainId(const Thread::Id& id)
{
	// this is only called once when the program starts and no other threads exist yet.
	// So there is no need for any synchronization or mutexes.
	getMainIdRef() = id;
}

bool Thread::isCurrentMain()
{
	return getCurrentId() == getMainId();
	
}

}
