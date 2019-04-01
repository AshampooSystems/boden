path: tree/master/framework/ui/include/bdn/
source: ViewCoreFactory.h

# ViewCoreFactory

View Core factory

## Declaration

```C++
class ViewCoreFactory : public bdn::Factory<std::shared_ptr<View::Core>, std::shared_ptr<ViewCoreFactory>>,
                        public std::enable_shared_from_this<ViewCoreFactory>
```

## Create

* **std::shared_ptr<ViewCore\> createViewCore(const std::type_info &viewType);**

## Register

* **template <class CoreType, class ViewType\> void registerCoreType();**

	Register a new Core type

## Context

* **template <class T\> static std::shared_ptr<T\> getContextStackTop()**

	Returns the top of the Context stack

* **static void pushContext(std::shared_ptr<bdn::UIContext\> &context);**

	Pushes a new Context to the top of the stack

* **static void popContext();**

	Removes the top context from the stack

## Relationships

Inherits from bdn::Factory and [std::enable_shared_from_this](https://en.cppreference.com/w/cpp/memory/enable_shared_from_this).
