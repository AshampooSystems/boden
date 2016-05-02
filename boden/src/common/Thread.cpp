#include <bdn/init.h>
#include <bdn/Thread.h>

namespace bdn
{
	

Thread::Id Thread::getCurrentId()
{
	return std::this_thread::get_id();
}


Thread::Id Thread::getMainId()
{
	// The main ID is set only once and then never changed again.
	// So there is no need for any synchronization or mutexes.
	return *getMainIdRef();
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