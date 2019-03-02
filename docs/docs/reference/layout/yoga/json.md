# JSON support

[FlexStylesheet](stylesheet.md) can be easily de-/serialized to/from json using the [nlohmann::json](https://github.com/nlohmann/json) library.

## Linking

The json helper functions are only enabled if the C++ define "BDN_HAS_NLOHMANN_JSON" is defined. This happens automatically if you link the Boden provided library:

```CMake
# In you CMakeLists.txt
target_link_library(MyApp PRIVATE nlohmann::json)
```

If you want to use your own version of the nlohmann json library, simply create your own define:
```CMake
# In you CMakeLists.txt
target_compile_definitions(MyApp PRIVATE BDN_HAS_NLOHMANN_JSON)
```

## Serializing

```C++
FlexStylesheet stylesheet;
// ... set some options

nlohmann::json serializedStyleSheet = stylesheet;
std::cout << serializedStyleSheet.dump(1) << std::endl;
```

Output:
```Json
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

Applying json to your stylesheet is simple with the build in **FlexJson()** and **FlexJsonString()** functions.

You can either use a json object:

```C++
nlohmann::json jsonStylesheet = 
	{{"direction", FlexStylesheet::Direction::Column},
     {"flexGrow", 1.0},
     {"flexShrink", 1.0},
     {"alignItems", FlexStylesheet::Align::Stretch},
     {"padding", {{"all", 20.0}}},
     {"margin", {{"all", 2.0}}}};

container->setLayoutStylesheet(FlexJson(jsonStylesheet));
```

Or you can use a string:

```C++
String json = "{ \"direction\" : \"Row\" }";
container->setLayoutStylesheet(FlexJsonString(json));
```

