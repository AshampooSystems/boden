path: tree/master/framework/foundation/include/bdn/property
source: Streaming.h

# Streaming (Property)

Allows you to create a [String](string.md) property that chains multiple properties and values together.

## Declaration

```C++
namespace bdn {
	class Streaming
}
```

## Example

```c++
Property<int> integerProperty = 10;
Property<String> type = "posts"s;
Property<String> streamingProperty(
	Streaming() << "There are "s << integerProperty << " " << type);

std::cout << streamingProperty.get() << std::endl; // "There are 10 posts"

integerProperty = 42;
type = "messages"s;

std::cout << streamingProperty.get() << std::endl; // "There are 42 messages"


```

## Operators

* **template <class T\> Streaming &operator<<(const Property<T\> &property)**

	Appends a `Property<T> property` to the stream

* **template <class T\> Streaming &operator<<(T value)**

	Appends a value `T value` to the stream