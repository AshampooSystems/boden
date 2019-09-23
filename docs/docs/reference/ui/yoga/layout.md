path: tree/master/framework/ui/modules/yoga/include/bdn/ui/yoga
source: Layout.h

# Layout

A Flexbox layout implementation based on [Facebook's Yoga library](https://yogalayout.com/playground/).

## Declaration

```c++
namespace bdn::ui::yoga
{
    class Layout : public ui::Layout
}
```

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
      }
     }};

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
        "padding" : {"all" : 20.0}
    });
```


## Stylesheet

The layout is configured using the [`View`](../view.md)'s [`stylesheet`](../view.md#properties) property.
The layout retrieves its setting from the `"flex"` entry.


### Defaults

If the [`View`](../view.md)'s stylesheet does not specify a value, the following defaults are used:

```json
{
  "flex" : {
    "alignContents": "Stretch",
    "alignItems": "Stretch",
    "alignSelf": "Auto",
    "direction": "Column",
    "flexBasis": null,
    "flexGrow": 0.0,
    "flexShrink": 1.0,
    "flexWrap": "NoWrap",
    "justifyContent": "FlexStart",
    "layoutDirection": "Inherit",
    "margin": {
      "all": null,
      "bottom": null,
      "left": null,
      "right": null,
      "top": null
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
    "maximumSize": {
      "height": null,
      "width": null
    },
    "minimumSize": {
      "height": null,
      "width": null
    },
    "aspectRatio": null
  }
}
```


### Container

* **`direction`**

	Specifies the layout direction.

	| Value | Direction |
	|---|---|
	| **Column** (default) | left to right |
	| ColumnReverse | right to left |
	| Row | top to bottom |
	| RowReverse | bottom to top |


* **`layoutDirection`**

	Specifies the layout anchor.

	| Value | Direction |
	|---|---|
	| **LTR** (default) | left to right |
	| RTL | right to left |

* **`flexWrap`** 

	Allows items to wrap to the next line if they don't fit.

	| Value | Result |
	|---|---|
	| **NoWrap** (default) | overflow / shrink  |
	| Wrap | move to next line |
	| WrapReverse | start at end, overflow into previous |


### Alignment

Available values for alignItems/alignSelf/alignContent:

| Value | Description |
|---|---|
| Auto | inherit parents alignItems  |
| FlexStart | Align towards parent start |
| Center | Align towards parent center  |
| FlexEnd | Align towards parent end |
| Stretch | fill parent secondary axis |
| Baseline | align to baseline of neighbouring items |
| SpaceBetween | move to next line |
| SpaceAround | move to next line |

* **`alignItems`**

	Alignment of children along the cross axis of the container.

	Defaults to `Stretch`.

* **`alignSelf`**

	Same as `alignItems` but for a single child (overrides the parents alignItems).

	Defaults to `Auto`.

* **`alignContent`**

	Alignment of lines along the cross-axis. Only affects containers with `flexWrap` enabled.
	
	Defaults to `Stretch`.

* **`justifyContent`**

	Aligns child nodes along the main axis. Possible values:

	| Value | Distribution |
	|---|---|
	| FlexStart (default) | start of the container |
	| Center | center of the container |
	| FlexEnd | end of the container |
	| SpaceEvenly | evenly across the container, remaining space split evenly around items |
	| SpaceBetween | evenly across the container, remaining space split between items |
	| SpaceAround | evenly across the container, remaining space split around the items |

	Defaults to `FlexStart`.


### Flex

* **`flexBasis`**

	Specifies the default size of the item along the containers main axis. 
	Similar to size/width for row containers or size/height for column containers.
	`flexBasis` can be specified in pixels or as percentage value (string):

	```json
	{ "flexBasis" : 100 },
	{ "flexBasis" : "100px" },
	{ "flexBasis" : "100%" }
	```


* **`flexGrow`**

	Specifies the fraction of a container's empty space the item should request. 
	The default value is 0.

* **`flexShrink`**

	Specifies how much to shrink an item if there is not enough space.


### Border

Margin and padding can be specified as pixels or percentage value (string):

```json
{ "margin" : {"all" : 100} },
{ "margin" : {"all" : "100px"} },
{ "margin" : {"all" : "100%"} }
```

As a shorthand for `all` you can also use this form:

```json
{ "margin" : 100 },
{ "padding" : 100 },
``` 

* **`margin`**

	Specifies the border **around** the item

* **`padding`**

	Specifies the border **inside** the item


### Size

Sizes are specified as dictionaries containing `width` and `height`.

| Boden | CSS equivalent |
|-------|----------------|
| size  | `width`, `height`  |
| minimumSize  | `min-width`, `min-height`  |
| maximumSize  | `max-width`, `max-height`  |

`minimumSize`/`maximumSize` can be specified as pixels or percentage value (string):

```json
{ "size" : {"width" : 100} },
{ "minimumSize" : {"width" : "100px"} },
{ "maximumSize" : {"width" : "100%"} }
```

* **`size`**

	Specifies the width and height of the item.

	* **`size/width`**
	* **`size/height`**

* **`minimumSize`**

	Specifies the minimum width and height of the item.

	* **`minimumSize/width`**
	* **`minimumSize/height`**

* **`maximumSize`**

	Specifies the maximum width and height of the item.

	* **`maximumSize/width`**
	* **`maximumSize/height`**


* **`aspectRatio`**

	Defines the aspect ratio.



