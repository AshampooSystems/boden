path: tree/master/framework/foundation/include/bdn/
source: Color.h

# Color

Represents an RGBA color. Can be de-/serialized from/to JSON.


## Example

```C++
#include <bdn/Color.h>
#include <bdn/Json.h>

int main() {
	bdn::Color color("royalblue");
	bdn::json j = color;

	return 0;
}
```

## Declaration

```C++
namespace bdn {
	class Color
}
```

## Constructor

* **Color()**

	Creates an opaque black color.

* **Color(std::string color)**

	Creates a color from a string. You can specify the color via a hex value: `#RRGGBBAA` or `#RGBA`.
	If you omit the alpha value it will default to opaque. You can also use up to [CSS Level 4](https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#Color_keywords) color names including `transparent`.

* **constexpr Color(std::array<double, 4> array)**

	Initializes the color with the given array of floating point component values in range [0,1] (r,g,b,a).

* **constexpr Color(double r, double g, double b, double a = 1.0f)**

	Initializes the color with the given floating point component values in range [0,1].

* **constexpr Color(uint32_t color)**

	Initializes the color with the give `uint32_t` value. Red is stored in the highest 8 bits, alpha is stored in the lowest 8 bits.


## Conversion

* **static Color fromAny(std::any anyColor)**

	Converts an std::any to Color. Throws std::bad_any_cast if no conversion possible.

	| std::any type  | calls |
	|---|---|
	| Color  | Color(std::any_cast<Color\>(any))  |
	| std::string | Color(std::any_cast<std::string\>(any)) |
	| json  | (Color)std::any_cast<json\>(any)  |

* **static Color fromIntAlphaFirst(uint32_t color)**

	Same as `Color(uint32_t)` but takes the alpha value from the highest 8 bits.

## RGBA

* **double red() const**

	Returns the red component as a floating point value in range [0,1].

* **double green() const**

	Returns the green component as a floating point value in range [0,1].

* **double blue() const**

	Returns the blue component as a floating point value in range [0,1].

* **double alpha() const**

	Returns the alpha component as a floating point value in range [0,1].

## Array access

* **std::array<double, 4> asArray() const**

	Returns the components as an `std::array` of doubles.

* **template <class T> std::array<T, 4> asIntArray() const**

	Returns the components as an `std::array<T>` where `T` is the int type to convert to. The internal floating point values are scaled from 0 to `std::numerical_limits<T>::max()`.

## Int32 access

* **uint32_t asInt() const** 

	Returns all components as one `uint32_t` in the form `0xRRGGBBAA`.

* **uint32_t asIntAlphaFirst() const**

	Returns all components as one `uint32_t` in the form `0xAARRGGBB`.
