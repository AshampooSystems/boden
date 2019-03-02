# Boden layout system

Boden supports customizable layouts via its [`View`](../ui/view.md) Baseclass.

Currently the only implementation provided is a flexbox style Layout based on the yoga library.

## Setting a Layout

```C++
auto window = std::make_shared<bdn::Window>();
window->setLayout(std::make_shared<bdn::yogalayout::Layout>());
```

( see [Yoga](yoga/summary.md) for more examples )