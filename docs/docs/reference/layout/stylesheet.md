# LayoutStylesheet

## Declaration

```C++
class LayoutStylesheet
```

## Type

* **String type()**

	Returns the type of the stylesheet.

	As a workaround to allow aggregate initialization this is not implemented as a virtual function, but instead expects the first member of a sub-class 
	to be a const char* containing the name of its type.

	```C++
	String type() { return *((const char **)this); }
	```
		
* **bool isType(String typeName)**

	Checks the type.