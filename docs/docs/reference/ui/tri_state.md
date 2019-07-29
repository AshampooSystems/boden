path: tree/master/framework/ui/include/bdn/ui
source: TriState.h

# TriState

Represents the on, off, or mixed state of a [Checkbox](checkbox.md).

## Definition

```c++
namespace bdn::ui {
	enum TriState
	{
	    Off,
	    On,
	    Mixed
	};
}
```

## Enum Values

* **TriState::Off**

	The control is in off state.

* **TriState::On**

	The control is in on state.

* **TriState::Mixed**

	The control is in mixed state representing a combination of other on/off controls, some of which are turned on and some of which are turned off currently.