path: tree/master/framework/ui/include/bdn/
source: GlobalStack.h

# GlobalStack

An implementation of a `thread_local` global static stack.

## Declaration

```C++
namespace bdn {
	template<class T, T(*defaultFunction)() = nullptr>
	class GlobalStack
}
```

## Example

```C++
#include <bdn/GlobalStack.h>

int defaultInt() {
    return 0;
}

int main() {
    using MyStack = bdn::GlobalStack<int, &defaultInt>;
 
    std::cout << MyStack::top() << std::endl; // "0"
    MyStack::push(10);    
    std::cout << MyStack::top() << std::endl; // "10"
    MyStack::pop();    
    std::cout << MyStack::top() << std::endl; // "0"
}
```

## Template arguments

* **T**

	The type stored in the stack

* **T(\*defaultFunction)()**

	A function pointer that is called if the stack is empty during a call to `top()` 

## Accessing the stack

* **static T& top()**

	Returns the top entry of the current threads stack.
	If the stack is empty and a `defaultFunction` was set, a new entry will be pushed onto the stack by calling `defaultFunction` and pushing the result onto the stack.
	If no `defaultFunction` was set an `std::runtime_error` will be thrown.

## Changing the stack

* **static void push(T&& entry)**

	Pushed the `entry` to the top of the current threads stack

* **static void pop()**

	Removes the top entry from the the current threads stack.
