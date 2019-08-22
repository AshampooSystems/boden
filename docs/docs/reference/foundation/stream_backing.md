path: tree/master/framework/foundation/include/bdn/property
source: StreamBacking.h

# StreamBacking

Allows you to create a std::string property that chains multiple properties and values together.

## Declaration

```C++
namespace bdn {
	class StreamBacking
}
```

## Example

```c++
#include <bdn/property/Property.h>

using namespace bdn;

Property<int> integerProperty = 10;
Property<std::string> type = "posts"s;
Property<std::string> streamingProperty(
	StreamBacking() << "There are "s << integerProperty << " " << type);

std::cout << streamingProperty.get() << std::endl; // "There are 10 posts"

integerProperty = 42;
type = "messages"s;

std::cout << streamingProperty.get() << std::endl; // "There are 42 messages"


```

## Operators

* **template <class T\> StreamBacking &operator<<(const [Property](property.md)<T\> &property)**

	Appends a `Property<T> property` to the stream

* **template <class T\> StreamBacking &operator<<(T value)**

	Appends a value `T value` to the stream