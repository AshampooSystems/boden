# Stack

Simple Button control

## Declaration

```C++
class Stack : public View
```

## Example

```C++
#include <bdn/Button.h>
#include <bdn/Stack.h>
// ...
auto stack = std::make_shared<Stack>();
auto firstButton = std::make_shared<Button>();

firstButton->label = "Click me!";
firstButton->onClick() += [=](auto) {
	auto secondButton = std::make_shared<Button>();
	secondButton->label = "Go back";
	secondButton->onClick() += [=](auto) { stack->popView(); }
	stack->pushView(secondButton, "Second Page");
};

stack->pushView(firstButton, "First Page");

```

## Stack manipulation

* **void pushView(std::shared_ptr<View\> view, String title)**

	Pushes the *view* with a *title* to the top of the stack. 

* **void popView()**

	Pops the top most View from the stack

## State

* **const std::deque<StackEntry\> &stack() const** 

	* Returns the current Views on the stack


## Structs

```C++
struct StackEntry
{
    std::shared_ptr<View> view;
    String title;
};
```

## Relationships

Inherits from [`View`](view.md).

