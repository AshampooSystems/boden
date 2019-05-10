# Flexbox Layouts

Boden provides a Flexbox layout implementation based on [Facebook's Yoga library](https://github.com/facebook/yoga).

## Example

First, create the layout object and apply it to the window:

```C++
auto window = std::make_shared<bdn::ui::Window>();
window->setLayout(std::make_shared<bdn::ui::yoga::Layout>());
```

Then set the stylesheet:

```C++ tab="C++"
nlohmann::json jsonStylesheet = 
    {{"flex",
      {{"direction", FlexStylesheet::Direction::Column},
       {"flexGrow", 1.0},
       {"flexShrink", 1.0},
       {"alignItems", FlexStylesheet::Align::Stretch},
       {"padding", {{"all", 20.0}}},
       {"margin", {{"all", 2.0}}}}}};

window->styleSheet = jsonStylesheet;
```

```C++ tab="Stringify"
window->styleSheet = 
    FlexJsonStringify(
    {
        "direction" : "Column", 
        "flexGrow" : 1.0, 
        "flexShrink" : 1.0, 
        "alignItems" : "Stretch",
        "padding" : {"all" : 20.0},
        "margin" : {"all" : 2.0}
    });
```


## Resources

To get familiar with the Flexbox Layout we recommend you to have a look at the <a href="https://yogalayout.com/playground" target="_blank">Yoga playground</a>.
