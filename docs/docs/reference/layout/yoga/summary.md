# Yoga layout

Boden provides a Layout implementation based on Facebooks yoga library.

## Example

First, create a Layout and apply it to the window

```C++
auto window = std::make_shared<bdn::Window>();
window->setLayout(std::make_shared<bdn::yogalayout::Layout>());
```

Now set a stylesheet. You can either use streaming style to specify the options ...

```C++
window->setLayoutStylesheet(FlexMarginBottom(5)
	<< FlexDirection(FlexStylesheet::Direction::Row) 
    << FlexAlignItems(FlexStylesheet::Align::Center)
    << FlexJustifyContent(FlexStylesheet::Justify::SpaceBetween)
    << FlexWrap(FlexStylesheet::Wrap::Wrap));
```

or aggregate initialization ... ( does not work in Visual Studio though! )

```C++
window->setLayoutStylesheet(FlexStylesheet { 
	.flexDirection = FlexStylesheet::Direction::Row, 
    .alignItems = FlexStylesheet::Align::Center,
    .justifyContent = FlexStylesheet::Justify::SpaceBetween,
    .wrap = FlexStylesheet::Wrap::Wrap
	.margin.bottom = 5, 
});
```

( You can also use [Json](json.md) )

View::setLayoutStyleSheet is a convenience function that creates a shared_ptr and assigns it to
the *layoutStyleSheet* property.

```C++
template <class T> 
void setLayoutStylesheet(T sheet) { 
	layoutStylesheet = std::make_shared<T>(sheet); 
}
```

## Resources

To get familiar with the Flex Box Layout we recommend to have a look at the [Yoga playground](https://yogalayout.com/playground)
