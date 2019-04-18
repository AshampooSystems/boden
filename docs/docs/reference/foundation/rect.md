path: tree/master/framework/ui/include/bdn/
source: Rect.h

# Rect

Represents a two-dimensional rectangle comprised of position and size represented by `x`, `y`, `width`, and `height` floating point numbers with double precision.

## Declaration

```C++
struct Rect
```

## Public Member Variables

* **double x = 0;**

	A `double` representing the horizontal position of the `Rect` object. Defaults to `0`.

* **double y = 0;**

	A `double` representing the vertical position of the `Rect` object. Defaults to `0`.

* **double width = 0;**

	A `double` representing the width of the `Rect` object. Defaults to `0`.

* **double height = 0;**

	A `double` representing the height of the `Rect` object. Defaults to `0`.

## Creating a Rect Object

* **Rect()**

	Default-constructs a `Rect` object with `x`, `y`, `width`, and `height` initialized to `0`.

* **Rect(const [Point](point.md) &pos, const [Size](size.md) &size)**

	Constructs a `Rect` object with the given position and size.

* **Rect(double x, double y, double width, double height)**

	Constructs a `Rect` object with the given `x`, `y`, `width`, and `height`.

## Retrieving Position and Size

* **[Point](point.md) position() const**

	Returns the rect's position as a [Point](point.md) object.

* **[Size](size.md) size() const**

	Returns the rect's size as a [Size](size.md) object.

