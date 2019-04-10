path: tree/master/framework/ui/include/bdn/
source: DispatchQueue.h

# DispatchQueue

A DispatchQueue can execute arbitrary functions on a thread.

## Declaration

```C++
class DispatchQueue
```

## Types

* **using Function = std::function<void()\>**
* **using Clock = std::chrono::steady_clock**
* **using TimePoint = Clock::time_point**

## Constructor

* **DispatchQueue(bool slave = false)**

	If slave is `false`, create an internal thread for the DispatchQueue. 

## Enqueue

* **void dispatchSync([Function](#types) function)**

	Dispatches a `function`on the DispatchQueue thread and waits for it to finish.

* **void dispatchAsync([Function](#types) function)**

	Dispatches a `function` on the DispatchQueue thread.

* **template <\> void dispatchAsyncDelayed(std::chrono::duration<\> delay, [Function](#types) function)**

	Dispatches a `function` to run on the DispatchQueue thread after the `delay`.

## Timer

* **template<\> createTimer(std::chrono::duration<\> interval, std::function<bool()\> timer)**

	Creates a timer that will run `timer` repeatedly on the DispatchQueue's thread every `interval` until `timer` returns false.

## Processing

* **void enter()**

	Only allowed if `slave` was true during construction. Starts the processing until `cancel` is called

* **void cancel()**

	Stops processing

* **void executeSync()**

	Execute a single item.