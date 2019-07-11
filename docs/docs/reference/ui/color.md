path: tree/master/framework/ui/include/bdn/ui
source: Color.h

# Color

Holds an RGBA color. Can be de-/serialized to json.


## Example

```C++
#include <bdn/ui/Color.h>
#include <bdn/Json.h>

int main() {
	bdn::ui::Color color("royalblue");
	bdn::json j = color;

	return 0;
}
```

## Declaration

```C++
namespace bdn::ui {
	class Color
}
```

## Constructor

* **Color()**

	Creates an opague black Color

* **Color(String color)**

	Creates a color from a string. You can specify the color via a hex value: `#RRGGBBAA` or `#RGBA`.
	If you omit the alpha value it will default to opague. You can also use up to [CSS Level 4](https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#Color_keywords) color names including `transparent`.

* **constexpr Color(std::array<float, 4> array)**

	Initializes the Color with the given array (r,g,b,a)

* **constexpr Color(float r, float g, float b, float a = 1.0f)**

	Initializes the Color with the given values.

* **constexpr Color(uint32_t color)**

	Initializes the color with the give `uint32_t` value. Red is stored in the highest 8 bits, the alpha is stored in the lowest 8 bits.


## RGBA

* **float red() const**

	Returns the red component as a float between 0 and 1

* **float green() const**

	Returns the green component as a float between 0 and 1

* **float blue() const**

	Returns the blue component as a float between 0 and 1

* **float alpha() const**

	Returns the alpha component as a float between 0 and 1

## Array access

* **std::array<float, 4> asArray() const**

	Returns the components as an `std::array` of floats

* **template <class T> std::array<T, 4> asIntArray() const**

	Returns the components as an `std::array<T>`. The internal float values are scaled from 0 to `std::numerical_limits<T>::max()`

## Int32 access

* **uint32_t asInt() const** 

	Returns all components as one `uint32_t` as `0xRRGGBBAA` 

* **uint32_t asIntAlphaFirst() const**

	Returns all components as one `uint32_t` as `0xAARRGGBB` 
