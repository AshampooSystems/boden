path: tree/master/framework/ui/include/bdn/
source: Notifier.h

# Notifier

A container that holds std::function<...\> objects that can be called using the *notify(...)* function.

## Declaration

```C++
template<class... Arguments>
class Notifier
```

## Types

* **using Subscription = size_t**

	An ID referencing a specific subscription

* **using Target = std::function<void(Arguments...)\>**

	The type of a callback

## Subscribe

* **Subscription subscribe(Target target)**

	Adds a new subscription

* **Notifier<Arguments...\> operator+=(Target target)**

	Convenience function to add a new subscription

## Unsubscribe

* **void unsubscribe(Subscription subscription)**
	
	Unsubscribe a specific subscription

* **void unsubscribeAll()**

	Removes all subscriptions

!!! note
	It is safe and valid to unsubscribe during a notify() call

## Notify

* **void notify(*Arguments*... arguments)**

	Notifies all registered subscriptions