path: tree/master/framework/ui/include/bdn/
source: AttributedString.h

# AttributedString

A string that allows you to set attributes for text ranges. Attributed strings are commonly used to apply formatting to a string.

## Declaration

```C++
namespace bdn {
	class AttributedString
}
```

## Construction

To create an instance of `AttributedString` simply call `std::make_shared<AttributedString>()`. A special implementation of `std::make_shared` will create the platform specific version of an `AttributedString`. 

## Content

* **virtual bool fromHTML(const String &html)**

	Replaces the contents of the `AttributedString` with the given `html` markup code.

* **virtual String toHTML() const**

	Returns the contents as a string containing HTML markup code.


## Types

* **CreatorStack**

	A [GlobalStack](global_stack.md) used to create platform specific versions of `AttributedString`.