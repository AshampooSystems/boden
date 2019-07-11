path: tree/master/framework/ui/include/bdn/
source: Text.h

# Text

A variant of [String](string.md) and std::shared_ptr<[AttributedString](attributed_string.md)>.


## Declaration

```C++
namespace bdn {
	class Text : public std::variant<String, std::shared_ptr<AttributedString>>
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

* **String operator()**

	Returns a [String](string.md) whether the variant holds a [String](string.md) or an [AttributedString](attributed_string.md).

* **bool operator!=(const bdn::Text &lhs, const bdn::Text &rhs)**

	Checks for inequality.

## Stylesheet

You can de-/serialize Text objects into json taking two forms.

* A single string results in a [String](string.md)
* A dictionary of the form `{"html" : <string> }` results in an [AttributedString](attributed_string.md) filled via [AttributedString::toHtml/AttributedString::fromHtml](attributed_string.md#content)
