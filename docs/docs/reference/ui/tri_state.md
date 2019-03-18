# TriState

Represents the on, off, or mixed state of a [Checkbox](checkbox.md).

## Definition

	enum TriState
	{
	    off,
	    on,
	    mixed
	};

## Enum Values

* **TriState::off**

	The control is in off state.

* **TriState::on**

	The control is in on state.

* **TriState::mixed**

	The control is in mixed state representing a combination of other on/off controls, some of which are turned on and some of which are turned off currently.