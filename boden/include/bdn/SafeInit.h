#ifndef BDN_SafeInit_H_
#define BDN_SafeInit_H_

#include <string>

namespace bdn
{


/** Base class for SafeInit. Usually not used directly.*/
class SafeInitBase
{
public:


	/** Returns the global mutex object that all SafeInit instances use to
		make the initialization safe.
		*/
	static Mutex& getGlobalMutex();

	static void _ensureReady()
	{
		// ensure that the global mutex object exists.
		getGlobalMutex();
	}

protected:
	enum class State : uint64_t
	{
		ok = 0x8c16d8db5274b9ea,
		error = 0x07e2801faa3098df,
		destructed = 0x788d0da5f79a5214 
	};

	void gotInitError(std::exception& e);
	void throwError();
	void threadYield();

	bool isUninitialized() const
	{
		return (_state!=State::ok && _state!=State::error && _state!=State::destructed);
	}



#if BDN_PLATFORM_DOTNET
	// std::exception_ptr is not supported when we compile for .NET.
	// Just store the error message instead.
	std::string _errorMessage;

#else
	std::exception_ptr _error;

#endif

	State	_state;
};

	

/** Takes care of the thread-safe initialization of global (static) variables.

	It is recommended to use the #BDN_SAFE_STATIC macro instead of using this
	class directly.

	
	The SafeInit class is primarily intended for static variables that are declared in functions.
	These get initialized when the function is first called.
	
	Some compilers do not implement static variable construction in a thread-safe manner
	(even though the C++11 standard actually demands this).

	When you do NOT use SafeInit with these compiler, then if multiple
	threads call the function with the local static variable at the same time,
	then it can happen that the initialization is run multiple times concurrently
	(potentially resulting in corrupted data and crashes),
	or that one thread starts using a half-constructed object.

	SafeInit prevents such issues and makes sure that the object is constructed
	only once and that construction is fully finished before it gets used.

	Another nice benefit of using SafeInit is that if an exception occurs during construction,
	then the exception will be thrown every time get() is called. So there is no way to accidentally
	use an object whose constructor has failed.

	The template parameter T can be any type (even a simple type like int or bool). If it is
	a simple type then the variable will be zero-initialized when no constructor parameters
	are given. You can also specify the initial value, if you like.

	The static object is allocated with new.

	When the program exits the global object will be destroyed/released. If it is derived
	from #bdn::Base then the object's reference count is decreased. So if some other object still
	holds a reference then the object actually remains alive until that remaining reference is released
	as well.
	If the object type is not derived from bdn::Base then the object is destroyed with delete when
	the program exits.
	
	Usage example:

	\code
	class MyClass : public Base
	{
	public:
		... some code ...

		// example for a static class instance
		static MyClass* getGlobalMyClass();

		// example for a static int
		static int& getGlobalInt();
	};


	// in the CPP file
	MyClass* MyClass::getGlobalMyClass();	
	{
		static SafeInit<MyClass> init(...constructor parameters...);

		return init.get();
	}

	static int& MyClass::getGlobalInt()
	{
		// the integer will be initialized to 17 on the first call of the function.
		static SafeInit<int> init( 17 );

		return *init.get();
	}

	\endcode

	*/
template<class T, class ... Arguments >
class SafeInit : public SafeInitBase
{
public:
	SafeInit(Arguments ... args)
	{
		// this constructor might be called by multiple threads at the same time.

		// The C++11 standard states that concurrent construction in multiple threads
		// must be thread-safe. Unfortunately, not all compilers implement this requirement.

		// So if we have one of those compilers then we must lock a mutex here to be safe.
#if BDN_COMPILER_STATIC_CONSTRUCTION_THREAD_SAFE==0

		// This mutex is shared by all SafeGlobal instances.
		// However, since it is only locked during construction (i.e. on first use), the performance
		// penalty is negligible.
		MutexLock lock( getGlobalMutex() );		
#endif

		// note that this part CAN actually be executed multiple times for the same object,
		// if the compiler does not implement the safe statics properly.
		// So we have to protect against that.
		if(isUninitialized())
		{
			try
			{
				_pInstance = rawNew<T>(args...);

				_state = State::ok;
			}
			catch(std::exception& e)
			{
				_pInstance = nullptr;

				gotInitError(e);
			}
		}
	}

	~SafeInit()
	{
		if(_state==State::ok)
		{
			_state = State::destructed;

			try
			{
				deleteOrReleaseRef(_pInstance);
			}
			catch(std::exception&)
			{
				// ignore.
			}
			_pInstance = nullptr;			
		}
	}

	T* get()
	{
		while(isUninitialized())
		{
			// this can actually happen when multiple threads try to construct the safe global at the same time.
			// Depending on the C++ compiler, the "second" thread will either also call the constructor, or it 
			// will immediately skip over the constructor and continue with the code after it.
			// If the second thread also calls the constructor then our constructor implementation will catch that and
			// make the second thread wait until the first thread has finished construction.
			// If the second thread skips over the constructor then it can actually happen that get is called before the
			// construction is complete. In that case we have to wait.
			threadYield();
		}

		if(_state!=State::ok)
			throwError();
		
		return _pInstance;
	}

protected:
	// we cannot use a smart pointer here because our constructor
	// might be called concurrently from different threads. Since member
	// construction happens before we can lock a mutex that means that
	// we cannot have any members with constructors. Otherwise the concurrent
	// constructor calls might overwrite each other's work.
	// Also, by using normal pointers it allows T to also be a simple type
	// or a class not derived from Base.
	T*				_pInstance;	
};


}

#endif

