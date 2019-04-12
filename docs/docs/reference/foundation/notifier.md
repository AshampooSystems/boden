path: tree/master/framework/foundation/include/bdn/
source: Notifier.h

# Notifier

Represents a signal used to notify observers of events and value changes.

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