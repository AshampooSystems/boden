# Yoga layout

Boden provides a Layout implementation based on Facebooks yoga library.

## Example

First, create the Layout and apply it to the window

```C++ hl_lines="2"
auto window = std::make_shared<bdn::Window>();
window->setLayout(std::make_shared<bdn::yogalayout::Layout>());
```

Now set a stylesheet.

```C++ tab="C++ Streaming"
window->setLayoutStylesheet(FlexMarginBottom(5)
	<< FlexDirection(FlexStylesheet::Direction::Row) 
    << FlexAlignItems(FlexStylesheet::Align::Center)
    << FlexJustifyContent(FlexStylesheet::Justify::SpaceBetween)
    << FlexWrap(FlexStylesheet::Wrap::Wrap));
```

```C++ tab="C++ Json"
nlohmann::json jsonStylesheet = 
	{{"direction", FlexStylesheet::Direction::Column},
     {"flexGrow", 1.0},
     {"flexShrink", 1.0},
     {"alignItems", FlexStylesheet::Align::Stretch},
     {"padding", {{"all", 20.0}}},
     {"margin", {{"all", 2.0}}}};

container->setLayoutStylesheet(FlexJson(jsonStylesheet));
```

```C++ tab="C++ aggregate"
// Does not work in Visual Studio !
window->setLayoutStylesheet(FlexStylesheet { 
	.flexDirection = FlexStylesheet::Direction::Row, 
    .alignItems = FlexStylesheet::Align::Center,
    .justifyContent = FlexStylesheet::Justify::SpaceBetween,
    .wrap = FlexStylesheet::Wrap::Wrap
	.margin.bottom = 5, 
});
```

[View::setLayoutStyleSheet](../../ui/view.md#layout) is a convenience function that creates a *shared_ptr* and assigns it to
the [View::layoutStyleSheet](../../ui/view.md#Properties) property.

```C++
template <class T> 
void setLayoutStylesheet(T sheet) { 
	layoutStylesheet = std::make_shared<T>(sheet); 
}
```

## Resources

To get familiar with the Flex Box Layout we recommend to have a look at the <a href="https://yogalayout.com/playground" target="_blank">Yoga playground</a>
