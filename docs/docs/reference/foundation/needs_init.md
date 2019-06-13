path: tree/master/framework/ui/include/bdn
source: NeedsInit.h

# NeedsInit

NeedsInit implements a custom std::make_shared version that allow to create classes that need to have their `init()` function called after construction.

## Types

* **class NeedsInit**

	Helper type to force using of the specialized `make_shared` implementation

	```c++
	namespace bdn {
		class NeedsInit {};
	    constexpr static const NeedsInit needsInit;
	}
	```

* **struct has_init_method** 

	Helper type to determine if a class has a method called `init()`

	```c++
	namespace bdn
	{
	    template <typename T> struct has_init_method
	    {
	      private:
	        typedef std::true_type yes;
	        typedef std::false_type no;
	        template <typename U> static auto test(int) -> decltype(std::declval<U>().init(), yes());
	        template <typename> static no test(...);
	      public:
	        static constexpr bool value = std::is_same<decltype(test<T>(0)), yes>::value;
	    };
	}
	```


## Functions

* **std::shared_ptr<T\> make_shared(bdn::NeedsInit needsInit, Arguments... arguments)**

	Specialized `make_shared<>()` that calls `init()` after construction.

	```c++
	namespace std
	{
	    template <typename T, typename... Arguments, typename std::enable_if<bdn::has_init_method<T>::value, int>::type = 0>
	    std::shared_ptr<T> make_shared(bdn::NeedsInit needsInit, Arguments... arguments)
	    {
	        auto t = std::allocate_shared<T, std::allocator<T>>(std::allocator<T>(), needsInit, arguments...);
	        t->init();
	        return t;
	    }
	}
	```

