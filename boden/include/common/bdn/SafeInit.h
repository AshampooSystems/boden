#ifndef BDN_SafeInit_H_
#define BDN_SafeInit_H_

namespace bdn
{


/** Base class for SafeInit. Usually not used directly.*/
class SafeInitBase
{
public:


	/** Returns the global mutex object that all SafeInit instances use to
		make the initialization safe.

		*/
	static Mutex& getGlobalMutex()
	{
		// Note that the global mutex itself is NOT constructed in a thread-safe manner.
		// So if the very first use happens concurrently in two threads then we can get
		// bad behaviour.
		// However, our Thread class and App class make sure that the mutex is initialized
		// as early as possible, and before any threads are started with our functions.

		static Mutex mutex;

		return mutex;
	}

protected:
	enum class State : uint64_t
	{
		ok = 0x8c16d8db5274b9ea,
		error = 0x07e2801faa3098df,
		destructed = 0x788d0da5f79a5214 
	};

	void gotInitError();
	void throwError();
	void threadYield();

	bool isUninitialized() const
	{
		return (_state!=State::ok && _state!=State::error && _state!=State::destructed);
	}


	std::exception_ptr _error;

	State	_state;
};

	



/** Takes care of the thread-safe initialization of global (static) variables.

	This is primarily intended for static variables that are declared in functions.
	These get initialized when the function is first called.
	When you do not use SafeGlobal with these, then if multiple
	threads call the function at the same tim, it can happen that the initialization
	is run multiple times concurrently (potentially resulting in corrupted data and crashes),
	or that one thread starts using a half-constructed object.
	SafeGlobal prevents such issues and makes sure that the object is constructed
	only once and that construction is fully finished before it gets used.
	
	Example:

	\code
	class MyClass : public Base
	{
	public:
		... some code ...

		static P<MyClass> getSingleton()
		{
			static SafeInit<MyClass> init(...constructor parameters...);

			return init.get();
		}
	};
	\endcode

	*/
template<class T, class ... Arguments >
class SafeInit : public SafeInitBase
{
public:
	SafeInit(Arguments ... args)
	{
		// this constructor might be called by multiple threads at the same time.
		// So we do not do any construction here!
		// We have to expect that.	

		// first we lock a global mutex. This mutex is shared by all SafeGlobal instances.
		// However, since it is only locked during construction (i.e. on first use), the performance
		// penalty is negligible.

		MutexLock lock( getGlobalMutex() );

		// note that we cannot have members with constructors or initializers because then we would risk that
		// a concurrent constructor call in another thread could overwrite our initializated stuff.
		// So the _constructResult member will be uninitialized and have some undetermined value based on the
		// contents of the RAM when the program started. That means that it could by accident have one of the two defined values.
		// However, since it is a 64 bit value and the two defined values were randomly chosen, the chance that
		// the variable will have one of them by accident is astronomically small. So we can ignore this possibility.
		if(isUninitialized())
		{
			try
			{
				_pInstance = new(Base::RawNew::Use) T(args...);

				_state = State::ok;
			}
			catch(std::exception&)
			{
				_pInstance = nullptr;

				gotInitError();
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
				_pInstance->releaseRef();
			}
			catch(std::exception&)
			{
				// ignore.
			}
			_pInstance = nullptr;			
		}
	}

	P<T> get()
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

	T*				_pInstance;
	
};



}


#endif

