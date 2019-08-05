path: tree/master/framework/ui/include/bdn/
source: Text.h

# Text

A variant of std::string and std::shared_ptr<[AttributedString](attributed_string.md)>.


## Declaration

```C++
namespace bdn {
	class Text : public std::variant<std::string, std::shared_ptr<AttributedString>>
}
```

## Example

```C++
#include <bdn/Text.h>
#include <bdn/Json.h>

int main() {
	bdn::Text text;
	bdn::json j = JsonStringify({"text" : {"html" : "<a href=\"http://www.boden.io\">www.boden.io</a>"}});

	text = j.get<bdn::Text>();

	return 0;
}
```

## Operators

* **std::string operator()**

	Returns a std::string whether the variant holds a std::string or an [AttributedString](attributed_string.md).

* **bool operator!=(const bdn::Text &lhs, const bdn::Text &rhs)**

	Checks for inequality.

## Stylesheet

You can de-/serialize Text objects into json taking two forms.

* A single string results in a std::string
* A dictionary results in an [AttributedString](attributed_string.md) (see [AttributedString serialization](attributed_string.md#json) for details)

## Enums

### TruncateMode

Used to specify the way that text is truncated if it does not fit into a View.

```c++
enum class TruncateMode
{
    Head,    // ... at beginning of text
    Tail,    // ... at end of text
    Middle,  // ... in middle of text
    Clip,    // Just clip text that does not fit
    ClipWord // Clip the text just before the first word that no longer fits
};
```


