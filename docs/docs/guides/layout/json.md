# Flex JSON Stylesheets

The Layout stylesheet of a [View](../../reference/ui/view.md) is specified by the `flex` entry of its stylesheet. 

## Serializing

```C++ tab="C++"
std::cout << view->stylesheet->at("flex").dump(1) << std::endl;
```

```Json tab="Output"
{
 "alignContents": "Stretch",
 "alignItems": "Stretch",
 "alignSelf": "Auto",
 "aspectRatio": null,
 "direction": "Column",
 "flexBasis": null,
 "flexBasisPercent": null,
 "flexGrow": 1.0,
 "flexShrink": 1.0,
 "justifyContent": "FlexStart",
 "layoutDirection": "Inherit",
 "margin": {
  "all": null,
  "bottom": null,
  "left": null,
  "right": null,
  "top": null
 },
 "maximumSize": {
  "height": null,
  "width": null
 },
 "minimumSize": {
  "height": null,
  "width": null
 },
 "padding": {
  "all": null,
  "bottom": null,
  "left": null,
  "right": null,
  "top": null
 },
 "size": {
  "height": null,
  "width": null
 },
 "wrap": "NoWrap"
}
```


## De-serializing

Applying JSON to your stylesheet is simple with the built-in `FlexJsonStringify()` functions.

You can either use a JSON object:

```C++
nlohmann::json jsonStylesheet = 
	{{"direction", FlexStylesheet::Direction::Column},
     {"flexGrow", 1.0},
     {"flexShrink", 1.0},
     {"alignItems", FlexStylesheet::Align::Stretch},
     {"padding", {{"all", 20.0}}},
     {"margin", {{"all", 2.0}}}};

container->stylesheet = nlohmann::json{{"flex", jsonStylesheet}};
```

Or you can use a string:

```C++
container->stylesheet = FlexJsonStringify({ "direction": "Row" });
```

This is equivalent to writing:

```C++
String json = "{\"flex\" : { \"direction\" : \"Row\" } }";
container->stylesheet = JsonStringify(json);
```

