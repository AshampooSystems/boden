# Yoga layout

Boden provides a Layout implementation based on Facebooks yoga library.

## Example

```C++
auto window = std::make_shared<bdn::Window>();
window->title = "testapp";
window->geometry = bdn::Rect{0, 0, 400, 300};
window->setLayout(std::make_shared<bdn::yogalayout::Layout>());

window->setLayoutStylesheet(FlexMarginBottom(5) 
	<< FlexDirection(FlexStylesheet::Direction::Row) 
    << FlexAlignItems(FlexStylesheet::Align::Center)
    << FlexJustifyContent(FlexStylesheet::Justify::SpaceBetween)
    << FlexWrap(FlexStylesheet::Wrap::Wrap));

auto button = std::make_shared<bdn::Button>();
window->content = button;
```

