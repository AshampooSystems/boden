path: tree/master/framework/ui/include/bdn/
source: Text.h

# Text

A variant of [String](string.md) and std::shared_ptr<[AttributedString](attributed_string.md)>

## Declaration

```C++
namespace bdn {
	class Text : public std::variant<String, std::shared_ptr<AttributedString>>
}
```

## Operators

* **String operator()**

	Returns a [String](string.md) whether the variant holds a [String](string.md) or an [AttributedString](attributed_string.md).

* **bool operator!=(const bdn::Text &lhs, const bdn::Text &rhs)**

	Checks for inequality.
