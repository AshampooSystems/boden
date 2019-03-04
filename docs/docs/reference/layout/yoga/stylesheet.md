# FlexStylesheet

## Example

```C++
auto styleSheet = std::make_shared<FlexStylesheet>();
styleSheet->flexDirection = Direction::Row;

window->layoutStylesheet = styleSheet;
```

## Flex

* **Direction flexDirection** 
	
	Defines the direction ( Row vs. Column ) of the View children

	Defaults to: *Direction::Column*

* **LayoutDirection layoutDirection**

	Defines the layout direction ( left to right, or right to left )

	Defaults to: *LayoutDirection::Inherit*

* **Wrap flexWrap**

	Defines whether the contained Items should wrap if the available space
	is not sufficient

	Defaults to: *Wrap::NoWrap*

* **float flexGrow**

	How much of the free space the view should fill

	Defaults to: *0.0f*

* **float flexShrink = 1.0**

	The shrink factor of the View in case the parent does not have enough space

* **std::optional<float\> flexBasis**
* **std::optional<float\> flexBasisPercent**

## Alignment

* **Align alignSelf**

	Defaults to: *Align::Auto*

* **Align alignItems**

	Defaults to: *Align::Stretch*

* **Align alignContents**

	Defaults to: *Align::Stretch*

* **Justify justifyContent**

	Defaults to: *Justify::FlexStart*

### Padding

Defines the border applied on the inside of the View

* **std::optional<float\> padding.all**
* **std::optional<float\> padding.left**
* **std::optional<float\> padding.top**
* **std::optional<float\> padding.right**
* **std::optional<float\> padding.bottom**

### Margin

Defines the border applied on the outside of the View

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
