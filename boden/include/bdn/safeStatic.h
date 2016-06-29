#ifndef BDN_safeStatic_H_
#define BDN_safeStatic_H_

/** \def BDN_SAFE_STATIC(type, accessorFuncName)

	You should use BDN_SAFE_STATIC and BDN_SAFE_STATIC_IMPL (or BDN_SAFE_STATIC_THREAD_LOCAL_IMPL)
	when you need a global (static) variable.
	
	BDN_SAFE_STATIC generates a declaration for a static accessor function (the function's name is 
	specified as the second parameter). The accessor function returns a reference to
	a global object of the specified type.

	The type parameter specifies the of the static variable.

	Use BDN_SAFE_STATIC in the header file to declare the accessor function.

	A corresponding #BDN_SAFE_STATIC_IMPL statement must be included in the implementation
	cpp file. If you want a thread-local global variable then you can use BDN_SAFE_STATIC_THREAD_LOCAL_IMPL instead.

	Note that BDN_SAFE_STATIC actually simply creates a function declaration. If you do not like using a macro
	in your header (for example, because it messes up documentation generators) then you can also replace it with
	a normal function declaration and then only use BDN_SAFE_STATIC_IMPL or BDN_SAFE_STATIC_THREAD_LOCAL_IMPL in your CPP file.

	\code
	static type& accessorFuncName();
	\endcode


	IMPORTANT: Do not put the implementation part in a header!
	It might seem like a good idea to put the implementation part directly into the header and not have a CPP file at all
	BUT if you do that then you can end up with multiple versions of the static variable in your program (for example, when
	you create two DLLs that use the header, then each will have its own copy of the variable).
	Because of this you MUST put the BDN_SAFE_STATIC_IMPL or BDN_SAFE_STATIC_THREAD_LOCAL_IMPL part into a CPP file!

	Usage example:

	\code


	// ---- In the header file ----

	class MyClass
	{
	public:

		// declare the static accessor function
		BDN_SAFE_STATIC(int, getGlobalValue);

		// alternatively: declare the function directly, without the macro.
		// This is equivalent and can make the live for documentation generators
		// like doxygen easier.
		static int& getGlobalValue();
	};

	// ---- In the implementation CPP file of MyClass ----
		
	// add this line in the implementation CPP file of MyClass
	BDN_SAFE_STATIC_IMPL( int, MyClass::getGlobalValue );

	// if you want the integer to be a thread-local global then you would use this instead:
	BDN_SAFE_STATIC_THREAD_LOCAL_IMPL( int, MyClass::getGlobalValue );
	
	// optionally, you can instead also pass constructor parameters to initialize
	// the global variable. Just add those at the end of the BDN_SAFE_STATIC_IMPL
	// statement.
	BDN_SAFE_STATIC_IMPL( int, MyClass::getGlobalValue, 17 );	// initialize the global instance to 17

	// If MyClass is inside a namespace then the BDN_SAFE_STATIC_IMPL statement must also
	// be inside the namespace. For example:
	namespace myNamespace
	{
		BDN_SAFE_STATIC_IMPL( int, MyClass::getGlobalValue );
	}


	// ---- Somewhere else in your source code ---
	
	// You can use the accessor function like this:
	int& globalVal = MyClass::getGlobalValue();

	\endcode
*/
#define BDN_SAFE_STATIC(typeName, accessorFuncName) static typeName& accessorFuncName()


/** \def BDN_SAFE_STATIC_IMPL(type, qualifiedAccessorFuncName)

	Provides the actual implementation of a static accessor function
	that was declared with #BDN_SAFE_STATIC.

	type is the type of the global variable. This must be the same
	type as the one passed to BDN_SAFE_STATIC.

	qualifiedAccessorFuncName is the qualified(!) name of the accessor
	function. If the accessor function was declared in a class then
	the class name MUST be included here (like this: ClassName::FuncName ).

	If the accessor function or the class of the accessor function
	is part of a namespace then the BDN_SAFE_STATIC_IMPL statement
	must also be put inside the namespace braces.

	Optionally you can pass constructor parameters for the global variable
	at the end of the macro.
	
	If you do not specify any constructor parameters then the global variable will be default-initialized.
	If the type parameter is a class or struct then the corresponding default-constructor will be used.
	If it is a simple type like an int or bool then it will be zero-initialized.

	See #BDN_SAFE_STATIC for example code.
*/
#define BDN_SAFE_STATIC_IMPL(typeName, qualifiedAccessorFuncName, ...) \
	typeName& qualifiedAccessorFuncName() \
	{ \
		static bdn::SafeInit<typeName> init{__VA_ARGS__}; \
		return *init.get(); \
	}




/** \def BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(type, qualifiedAccessorFuncName)

	Like #BDN_SAFE_STATIC_IMPL, except that the variable will be a thread-local
	static instead of a normal static.

	With a thread-local static each thread will have its own instance of the variable.
	So when you cann the accessor function multiple times from the same thread then you will
	get the SAME object every time. If you call it from a different thread then you
	will get a different object.

	The thread local instances are automatically destroyed when their thread
	exits.
	
	Other than the fact that you get a different variable in each thread, this works exactly
	like #BDN_SAFE_STATIC_IMPL. See the #BDN_SAFE_STATIC_IMPL for more detailed information.
*/
#if BDN_HAVE_THREADS

	#if BDN_PLATFORM_OSX || BDN_PLATFORM_IOS
        // Apple's clang implementation does not support standard C++11 thread_local storage. So we have to use
        // a workaround here.
        #include <bdn/pthread/ThreadLocalStorage.h>

		#define BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(typeName, qualifiedAccessorFuncName, ...) \
			typeName& qualifiedAccessorFuncName() \
			{ \
				static bdn::SafeInit< bdn::pthread::ThreadLocalStorage<typeName> > init{__VA_ARGS__}; \
				return (typeName&)*init.get(); \
			}


	#elif BDN_PLATFORM_WINRT
		
		// on WinRT the thread local keyword does not work with objects that have a destructor.
		// So we use the manual implementation here. Note that we can use the one for the win32
		// API, as those parts of the API are supported on WinRT.
		#include <bdn/win32/ThreadLocalStorage.h>

		#define BDN_SAFE_STATIC_THREAD_LOCAL_IMPL( typeName, qualifiedAccessorFuncName, ...)\
			typeName& qualifiedAccessorFuncName() \
			{ \
				static bdn::SafeInit< bdn::win32::ThreadLocalStorage<typeName> > init{__VA_ARGS__}; \
				return (typeName&)*init.get(); \
			}

	#else

		// just use standard C++11 thread_local storage.
		// Note that we still use SafeInit, so that we get the special handling of exceptions during construction.
		#define BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(typeName, qualifiedAccessorFuncName, ...) \
			typeName& qualifiedAccessorFuncName() \
			{ \
				static thread_local bdn::SafeInit<typeName> init{__VA_ARGS__}; \
				return *init.get(); \
			}

    #endif

#else

    // we have no threads => thread local is the same as static
	// Note that we still use SafeInit, so that we get the special handling of exceptions during construction.
    #define BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(typeName, qualifiedAccessorFuncName, ...) \
			typeName& qualifiedAccessorFuncName() \
			{ \
				static bdn::SafeInit<typeName> init{__VA_ARGS__}; \
				return *init.get(); \
			}

#endif


#endif

