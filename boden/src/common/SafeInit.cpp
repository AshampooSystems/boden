#include <bdn/init.h>

#if !BDN_TARGET_DOTNET
#include <thread>
#endif

namespace bdn
{


void SafeInitBase::gotInitError(std::exception& e)
{
	_state = State::error;

#if BDN_TARGET_DOTNET
	// std::exception_ptr is not supported when we compile for .NET
	_errorMessage = e.what();

#else
	_error = std::current_exception();

#endif
}


void SafeInitBase::throwError()
{
	if(_state==State::error)
	{
		// throw the original error.	
#if BDN_TARGET_DOTNET
		throw std::exception(_errorMessage.c_str());
#else
		std::rethrow_exception( _error );
#endif
	}
	
	else if(_state==State::destructed)
		throw std::runtime_error("SafeInit object already destructed.");

	else
		throw std::runtime_error("SafeInit object has an invalid state.");
}

void SafeInitBase::threadYield()
{
#if BDN_TARGET_DOTNET
	System::Threading::Thread::Yield();

#else
	std::this_thread::yield();
#endif
}


}