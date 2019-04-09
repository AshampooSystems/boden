# Boden layout system

Boden supports customizable layouts via its [`View`](../../reference/ui/view.md) Baseclass.

Currently the only implementation provided is a flexbox style Layout based on the yoga library.

## Setting a Layout

```C++
auto window = std::make_shared<bdn::Window>();
window->setLayout(std::make_shared<bdn::yogalayout::Layout>());
```

( see [Yoga](yoga/summary.md) for more examples )

## Setting a stylesheet

```C++
auto container = std::make_shared<Container>();
container->setLayoutStylesheet(FlexJson({"grow", 1.0}));
```

( see [JSON Support](json.md) for more examples )
