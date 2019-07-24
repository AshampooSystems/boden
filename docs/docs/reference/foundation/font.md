path: tree/master/framework/foundation/include/bdn/
source: Font.h

# Font

Represents a font. Can be de-/serialized from/to JSON.


## Example

```C++
#include <bdn/Font.h>
#include <bdn/Json.h>

int main() {
	bdn::Font font;
	font.family = "Arial";
	font.size.type = Font::Size::Type::Pixels;
	font.size.value = 32;

	bdn::json j = font;

	return 0;
}
```

## Declaration

```C++
namespace bdn {
	class Font
}
```

## Conversion

* **static Font fromAny(std::any anyFont)**

	Converts an std::any to Font. Throws std::bad_any_cast if no conversion possible.

	| std::any type  | calls |
	|---|---|
	| Font  | Font(std::any_cast<Font\>(any))  |
	| json  | (Font)std::any_cast<json\>(any)  |

## Members

* **String family**

	The font family

* **Size size**

	The font size ( see [Size](#size) )

* **Style style = Style::Inherit**

	The font style

* **Weight weight = Weight::Inherit**

	The font weight.

* **Variant variant = Variant::Inherit**

	The font variant

## Inner types

### Size

```c++
struct Size
{
    enum class Type
    {
        Inherit = 0,
        Medium = 1,
        Small = 2,
        XSmall = 3,
        XXSmall = 4,
        Large = 5,
        XLarge = 6,
        XXLarge = 7,
        Percent = 8,
        Points = 9,
        Pixels = 10,
    };

    Type type = Type::Inherit;
    float value = 11.0f;
};
```

### Weight

```c++
enum Weight
{
    Inherit = -1,
    Normal = 400,
    Bold = 700
};
```

### Style

```c++
enum class Style
{
    Inherit,
    Normal,
    Italic
};
```

### Variant

```c++
enum class Variant
{
    Inherit,
    Normal,
    SmallCaps
};
```


