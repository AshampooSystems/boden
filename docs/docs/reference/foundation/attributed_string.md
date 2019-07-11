path: tree/master/framework/ui/include/bdn/
source: AttributedString.h

# AttributedString

Holds an attributed String.


## Declaration

```C++
namespace bdn {
	class AttributedString
}
```

## Construction

To create an instance of `AttributedString` simply call `std::make_shared<AttributedString>()`. A special implementation of `std::make_shared` will create the platform specific version of an `AttributedString`. 

## Content

* **virtual bool fromHtml(const String &html)**

	Replaces the contents of the `AttributedString` with the `html` code.

* **virtual String toHtml() const**

	Returns the contents encoded as Html.


## Types

* **CreatorStack**

	A [GlobalStack](global_stack.md) used to create platform specific versions of `AttributedString`.