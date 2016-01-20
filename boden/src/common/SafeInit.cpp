#include <bdn/init.h>

#include <thread>

namespace bdn
{


void SafeInitBase::gotInitError()
{
	_state = State::error;

	_error = std::current_exception();
}


void SafeInitBase::throwError()
{
	if(_state==State::error)
	{
		// throw the original error.	
		std::rethrow_exception( _error );
	}
	
	else if(_state==State::destructed)
		throw std::runtime_error("SafeInit object already destructed.");

	else
		throw std::runtime_error("SafeInit object has an invalid state.");
}

void SafeInitBase::threadYield()
{
	std::this_thread::yield();
}


}