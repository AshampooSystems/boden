# The Boden Layout System

The Boden Framework supports customizable layouts via its [`View`](../../reference/ui/view.md) base class.

The default implementation for layouts in Boden provides Flexbox layouts based on [Facebook's Yoga library](https://github.com/facebook/yoga).

## Setting a Layout

```C++
auto window = std::make_shared<bdn::ui::Window>();
window->setLayout(std::make_shared<bdn::ui::yoga::Layout>());
```

See [Yoga](yoga.md) for more examples.

## Setting a stylesheet

```C++
auto container = std::make_shared<bdn::ui::Container>();
container->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
```

See [JSON Support](json.md) for more examples.
