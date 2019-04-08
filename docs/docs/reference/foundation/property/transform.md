path: tree/master/framework/foundation/include/bdn/property
source: Transform.h

# Transform Backing

The transform backing allows you to create a property that represents a custom transformation from another property.

## Declaration

```C++
template<class T, class U>
class Transform
```

## Example

```c++
auto toFunc = [](int value) -> String {
    switch (value) {
    case 0:
        return "no"s;
    case 1:
        return "one"s;
    case 2:
        return "two"s;
    case 3:
        return "three"s;
    case 4:
        return "four"s;
    }
    std::ostringstream s;
    s << value;
    return s.str();
};

auto fromFunc = [](String value) -> int {
	if(value.empty())
		return 0;
	if(value == "no"s)
		return 0;
	if(value == "one"s)
		return 1;
	if(value == "two"s)
		return 2;
	if(value == "three"s)
		return 3;
	if(value == "four"s)
		return 4;
	int result = 0;
	std::istringstream s(value);
	s >> result;
	return result;
};

Property<int> integerProperty = 1;
Property<String> transformedProperty(Transform<String, int>{test, toFunc, fromFunc});
// transformedProperty now equals "one"s

integerProperty = 42;
// transformedProperty now equals "42"s

transformedProperty = "12345";
// integerProperty now equals 12345
```

## Types

* **using ToFunc = std::function<T(U)\>**
* **using FromFunc = std::function<U(T)\>**

## Constructor

* **Transform(const Property<U\> &p, ToFunc to, FromFunc from)**

	Creates a Transform object that can be passed to a Property<T>.
	Whenever the `Property<U>& p` changes, that value of the `Property<T>` will be changed to reflect 
	the value of the `Property<U>& p` based on the result of the `Backing::ToFunc to` and vice versa