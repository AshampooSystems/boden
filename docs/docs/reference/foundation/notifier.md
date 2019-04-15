path: tree/master/framework/foundation/include/bdn/
source: Notifier.h

# Notifier

Notifies subscribing callback methods about an event synchronously.

## Declaration

```C++
template<class... Arguments>
class Notifier
```

## Types

* **using Subscription = size_t**

	An ID referencing a specific subscription.

* **using Target = std::function<void(Arguments...)\>**

	The type of a callback.

## Subscribing to a Notifier

* **Subscription subscribe(Target target)**

	Subscribes the function specified by `target` to the notifier and returns a `Subscription` value. The returned `Subscription` may be persisted by the caller to later unsubscribe from the subscription again.

* **Notifier<Arguments...\> operator+=(Target target)**

	Convenience for adding a new subscription by using `operator +=`. If you need to unsubscribe the subscriber later on, use `subscribe` instead.

## Unsubscribing from a Notifier

* **void unsubscribe(Subscription subscription)**
	
	Unsubscribe the given subscription.

* **void unsubscribeAll()**

	Unsubscribe all subscriptions.

!!! note
	It is safe to unsubscribe during a notify() call

## Notifying Subscribers

* **void notify(*Arguments*... arguments)**

	Notifies all subscribers. Passes the given arguments to each subscriber.