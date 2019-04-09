path: tree/master/framework/foundation/include/bdn/property
source: Compare.h

# Compare

Helper class to define an operator `not_equal`. Used by [Property::set()](property.md#value) to determine if the value has changed.

The `const bool is_faked` is used to determine whether the result of `not_equal` is reliable. [Property::bind()](property.md#binding) will throw an std::logic_error if the user tries to create a Bidirectional binding, as that would result in an endless loop.

## Declaration

```C++
namespace bdn {
	template <class T> struct Compare
	{
	    static const bool is_faked = false;
	    static bool not_equal(const T &left, const T &right) 
	    	{ return left != right; }
	};

	template <class _fp> struct Compare<std::function<_fp>>
	{
	    static const bool is_faked = true;
	    static bool not_equal(const std::function<_fp> &left, 
	    					  const std::function<_fp> &right) 
	    	{ return true; }
	};
}
```
