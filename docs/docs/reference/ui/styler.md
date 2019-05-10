path: tree/master/framework/ui/include/bdn/ui
source: Styler.h

# Styler

Allows conditional changes of a [Views](./view.md) stylesheet.

Multiple conditional parts can be active, in which case they will be merged from first to last. 

## Example

```c++
#include <bdn/ui/Styler.h>
#include <bdn/ui/Button.h>

auto styler = std::make_shared<Styler>();
auto button = std::make_shared<Button>();

styler->setCondition("os", std::make_shared<Styler::equals_condition>(BDN_TARGET));

// If 'os' equals 'ios' the button will get a 5px padding
// The margin will always be 5px
_styler->setStyleSheet(addButton,
    JsonStringify(
    	[ 
    		{
    			"flex" : {"margin" : {"all" : 5}}
    		},
    		{ 
    			"if" : {"os" : "ios"}, 
    			"flex" : {"padding" : {"all" : 5}}
    		} 
    	]
 	)
);
```

## Declaration

```C++
namespace bdn::ui {
	class Styler
}
```

## Layout

* **void setStyleSheet(std::shared_ptr<View\> view, json stylesheet)**
	
	Sets the `stylesheet` for `view`. The stylesheet root must be an array.

* **void setCondition(String name, std::shared_ptr<condition\> condition)**

	Adds a new `condition` named `name` that stylesheets can match against.
	If a condition with `name` already exists it will be overriden and all
	registered View stylesheets will be updated if they are conditioned based
	on the name.

## Conditions

* **condition**

	Base class for conditions. Overload the `bool operator()` for your own custom conditions.

	```C++
    struct condition
    {
        virtual ~condition() = default;
        virtual bool operator()(const json::value_type &) = 0;
    };
    ```

* **equals_condition**

	Returns true if the value is equal to the target value

	```C++
	struct equals_condition : public condition
	{
	    equals_condition(json::value_type targetValue) 
	    	: _targetValue(targetValue) {}
	    bool operator()(const json::value_type &value) override 
	    { 
	    	return value == _targetValue; 
	    }
	    json::value_type _targetValue;
	};
	```

* **less_condition**

	Returns true if the value is less than the target value

	```C++
	struct less_condition : public condition
	{
	    less_condition(json::value_type targetValue) 
	    	: _targetValue(targetValue) {}
	    bool operator()(const json::value_type &value) override 
	    { 
	    	return value < _targetValue; 
	    }
	    json::value_type _targetValue;
	};
	```

* **greater_equal_condition**

	Returns true if the value is greater or equal to the target value

	```C++
	struct greater_equal_condition : public condition
	{
	    greater_equal_condition(json::value_type targetValue) 
	    	: _targetValue(targetValue) {}
	    bool operator()(const json::value_type &value) override 
	    { 
	    	return value >= _targetValue; 
	    }
	    json::value_type _targetValue;
	};
	```