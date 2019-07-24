path: tree/master/framework/ui/include/bdn/
source: AttributedString.h

# AttributedString

A string that allows you to set attributes for text ranges. Attributed strings are commonly used to apply formatting to a string.

## Declaration

```C++
namespace bdn {
	class AttributedString
}
```

## Construction

To create an instance of `AttributedString` simply call `std::make_shared<AttributedString>()`. A special implementation of `std::make_shared` will create the platform specific version of an `AttributedString`. 

## Serializing

* **virtual bool fromHTML(const String &html)**

	Replaces the contents of the `AttributedString` with the given `html` markup code.

* **virtual String toHTML() const**

	Returns the contents as a string containing HTML markup code.


## Json

* **void fromJson(const bdn::json& json)**

	Replaces the contents of the string with the data from `json`.

	HTML form:

	```json
	{"html" : <html-string>}
	```

	Raw form:

	```json
	{"string" : <raw string>,
	 "ranges" : [
	  {
	 	"start" : <int>,
	 	"length" : <int>,
	 	attributes: { <name> : <value>, ... }
	  }, 
	  ...
	 ]
	}
	```
	

* **bdn::json toJson() const**

	Returns the output of `toHTML()` wrapped in a json object.

## Attributes

* **virtual void addAttribute(String attributeName, std::any value, Range range)**

	Adds an attribute to the specified `range`. The value is converted using the types `fromAny` function.

	| Attribute Name | Type |
	|---|---|
	| `foreground-color` | [Color](color.md) |
	| `font` | [Font](font.md) |
	| `link` | [String](string.md) |
	| `baseline-offset` | `float` |

* **virtual void addAttributes(AttributeMap attributes, Range range)**

	Adds all attributes in `attributes` to the specified `range`

## Types

* **CreatorStack**

	A [GlobalStack](global_stack.md) used to create platform specific versions of `AttributedString`.