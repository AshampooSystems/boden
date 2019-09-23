path: tree/master/framework/foundation/include/bdn
source: Size.h

# Size

Represents a two-dimensional size comprised of `width` and `height` floating point numbers with double precision.

`Size` objects are used throughout the framework to work with measurements and layouts. By default, a size in Boden is used to represent unscaled coordinates (points) and might be translated to physical pixels as required by the framework.

## Declaration

```C++
namespace bdn {
	struct Size
}
```

## Public Member Variables

* **double width = 0;**

	A `double` representing the width of the `Size` object. Defaults to `0`.

* **double height = 0;**

	A `double` representing the height of the `Size` object. Defaults to `0`.

## Creating a Size Object

* **constexpr Size()**

	Constructs a `Size` object with `width` and `height` initialized to `0`.

* **constexpr Size(double width, double height)**

	Constructs a `Size` object with the given `width` and `height`.

## Representing Non-Sizes

* **static constexpr double componentNone();**

	Represents a size component set to a non-value. Non-values are represented as `std::numeric_limits<double>::infinity()`.

* **static constexpr Size none()**

	Returns a new `Size` object with `width` and `height` both initialized to `componentNone()`.

## Calculating with Sizes

* **Size operator+(const Size &o)**

	Adds the right hand size value to the left hand size value by performing component-wise addition.

* **Size &operator+=(const Size &o)**

	Adds the given right hand size value to the left hand size value by performing component-wise addition and stores the result in the left hand size value.

* **Size operator-(const Size &o)**

	Subtracts the right hand size value from the left hand size value by performing component-wise subtraction.

* **Size &operator-=(const Size &o)**

	Subtracts the given right hand size value from the left hand size value by performing component-wise addition and stores the result in the left hand size value.
