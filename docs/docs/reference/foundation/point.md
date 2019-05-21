path: tree/master/framework/ui/include/bdn/
source: Point.h

# Point

Represents a two-dimensional point comprised of `x` and `y` floating point numbers with double precision.

## Declaration

```C++
namespace bdn {
	struct Point
}
```

## Public Member Variables

* **double x = 0;**

	A `double` representing the horizontal position of the `Point` object. Defaults to `0`.

* **double y = 0;**

	A `double` representing the vertical position of the `Point` object. Defaults to `0`.

## Creating a Point Object

* **Point()**

	Constructs a `Point` object with `x` and `y` initialized to `0`.

* **Point(double x, double y)**

	Constructs a `Point` object with the given `x` and `y` coordinates.
