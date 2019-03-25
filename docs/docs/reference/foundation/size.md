# Size

Width / height container

## Declaration

```C++
class Size
```

## Member 

* **double width = 0;**
* **double height = 0;**

## Helper

* **static constexpr double componentNone();**
* **static constexpr Size none()**

## Constructor

* **constexpr Size(double width, double height)**

## Operators

* **Size operator-(const Margin &margin)**
* **Size &operator-=(const Margin &margin)**
* **Size operator+(const Margin &margin)**
* **Size &operator+=(const Margin &margin)**
* **Size operator+(const Size &o) const**
* **Size &operator+=(const Size &o)**
* **Size operator-(const Size &o)**
* **Size &operator-=(const Size &o)**

## Utility

* **void applyMinimum(const Size &minSize)**
* **void applyMaximum(const Size &maxSize)**
