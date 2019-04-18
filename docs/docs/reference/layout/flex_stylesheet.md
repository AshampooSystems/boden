path: tree/master/framework/layout/include/bdn
source: FlexStylesheet.h

# FlexStylesheet

Stylesheet for Flex based Layouts.

## Declaration

```C++
class FlexStylesheet : public layoutStylesheet
```

## Example

```C++
auto styleSheet = std::make_shared<FlexStylesheet>();
styleSheet->flexDirection = Direction::Row;
window->layoutStylesheet = styleSheet;
```

## Flex

* **[Direction](#enums) flexDirection** 
	
	Defines the direction (row vs. column) of the view's children.

	Defaults to: `Direction::Column`.

* **[LayoutDirection](#enums) layoutDirection**

	Defines the layout direction (left to right, or right to left).

	Defaults to: `LayoutDirection::Inherit`.

* **[Wrap](#enums) flexWrap**

	Defines whether the contained items should wrap if the available space
	is not sufficient.

	Defaults to: `Wrap::NoWrap`.

* **float flexGrow = 0.0**

	How much of the free space the view should fill.

	Defaults to: `0.0`.

* **float flexShrink = 1.0**

	The shrink factor of the View in case the parent does not have enough space.

    Defaults to: `1.0`.

* **std::optional<float\> flexBasis**
* **std::optional<float\> flexBasisPercent**

## Alignment

* **[Align](#enums) alignSelf**

	Defaults to: `Align::Auto`.

* **[Align](#enums) alignItems**

	Defaults to: `Align::Stretch`.

* **[Align](#enums) alignContents**

	Defaults to: `Align::Stretch`.

* **[Justify](#enums) justifyContent**

	Defaults to: `Justify::FlexStart`.

### Padding

Defines the space generated around the inside of the view.

* **std::optional<float\> padding.all**
* **std::optional<float\> padding.left**
* **std::optional<float\> padding.top**
* **std::optional<float\> padding.right**
* **std::optional<float\> padding.bottom**

### Margin

Defines the space generated around the outside of the view.

* **std::optional<float\> margin.all**
* **std::optional<float\> margin.left**
* **std::optional<float\> margin.top**
* **std::optional<float\> margin.right**
* **std::optional<float\> margin.bottom**


### Size

* **std::optional<float\> size.width**
* **std::optional<float\> size.height**
* **std::optional<float\> aspectRatio**


### Minimum Size

* **std::optional<float\> minimumSize.width**
* **std::optional<float\> minimumSize.height**

### Maximum Size

* **std::optional<float\> maximumSize.width**
* **std::optional<float\> maximumSize.height**

## Enums

```C++ tab="Direction"
enum class Direction
{
    Column,
    ColumnReverse,
    Row,
    RowReverse
};
```

```C++ tab="LayoutDirection"
enum class LayoutDirection
{
    Inherit,
    LTR,
    RTL
};
```

```C++ tab="Align"
enum class Align
{
    Auto,
    FlexStart,
    Center,
    FlexEnd,
    Stretch,
    Baseline,
    SpaceBetween,
    SpaceAround
};
```

```C++ tab="Justify"
enum class Justify
{
    FlexStart,
    Center,
    FlexEnd,
    SpaceBetween,
    SpaceAround,
    SpaceEvenly
};
```

```C++ tab="Wrap"
enum class Wrap
{
    NoWrap,
    Wrap,
    WrapReverse
};
```
