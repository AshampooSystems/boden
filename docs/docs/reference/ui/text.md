path: tree/master/framework/ui/include/bdn/ui
source: Text.h

# Text

A `std::variant` of [String](../foundation/string.md) and std::shared_ptr<[AttributedString](../foundation/attributed_string.md)>.


## Declaration

```C++
namespace bdn::ui {
	class Text : public std::variant<String, std::shared_ptr<AttributedString>>
}
```

## Example

```C++
#include <bdn/ui/Text.h>
#include <bdn/Json.h>

int main() {
	bdn::ui::Text text;
	bdn::json j = JsonStringify({
		"text" : 
			{"html" : "<a href=\"http://www.boden.io\">www.boden.io</a>"}
	});

	text = j.get<bdn::ui::Text>();

	return 0;
}
```

## Operators

* **String operator()**

	Returns a [String](../foundation/string.md) whether the variant holds a [String](../foundation/string.md) or an [AttributedString](../foundation/attributed_string.md).

* **bool operator!=(const bdn::ui::Text &lhs, const bdn::ui::Text &rhs)**

	Checks for inequality.

## Stylesheet

You can de-/serialize `Text` objects into JSON taking two forms:

* A single string results in a [String](../foundation/string.md).
* A dictionary results in an [AttributedString](../foundation/attributed_string.md) (see [AttributedString serialization](../foundation/attributed_string.md#json) for details).


