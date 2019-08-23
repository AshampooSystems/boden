path: tree/master/framework/ui/include/bdn/ui
source: textInputTypes.h

# AutocorrectionType

The type of autocorrection used by a [`TextField`](text_field.md) or other text input control.

## Definition

```C++
namespace bdn::ui
{
	enum class AutocorrectionType
	{
	    Default,
	    No,
	    Yes
	};	
}
```

## Enum Values

* **AutocorrectionType::Default**

	The text control uses the default autocorrection behavior defined by the platform.

* **AutocorrectionType::No**

	The text control should not apply autocorrection and autocomplete while typing. Also, no typing suggestions should be displayed to the user.

* **AutocorrectionType::Yes**

	The text control should display typing suggestions and apply autocorrection and autocomplete while typing.