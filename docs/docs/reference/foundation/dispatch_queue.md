path: tree/master/framework/ui/include/bdn/
source: DispatchQueue.h

# DispatchQueue

Queues and executes arbitrary functions on a thread.

## Declaration

```C++
namespace bdn {
	class DispatchQueue
}
```

## Types

* **using Function = std::function<void()\>**
* **using Clock = std::chrono::steady_clock**
* **using TimePoint = Clock::time_point**

## Creating a DispatchQueue Object

* **DispatchQueue(bool slave = false)**

	Constructs a dispatch queue. If `slave` is `false` (the default), creates and manages a thread for the dispatch queue. If `slave` is `true`, the queue will not create a thread on its own. This is handy if there is already a thread/run loop which should be integrated with the dispatch queue.

## Dispatching Methods to the Queue

* **void dispatchSync([Function](#types) function)**

	Dispatches a `function`on the dispatch queue thread and waits for it to finish.

* **void dispatchAsync([Function](#types) function)**

	Dispatches a `function` on the dispatch queue thread and returns immediately.

* **template <\> void dispatchAsyncDelayed(std::chrono::duration<\> delay, [Function](#types) function)**

	Dispatches a `function` to run on the dispatch queue thread after the `delay`.

## Creating Timers

* **template<\> createTimer(std::chrono::duration<\> interval, std::function<bool()\> timer)**

	Creates a timer that will run `timer` repeatedly on the dispatch queue's thread every `interval` until `timer` returns `false`.

## Controlling the Queue's Internal Processing

* **void enter()**

	Starts processing synchronously until `cancel` is called. Only allowed if `slave` was true during construction.

* **void cancel()**

	Stops processing as soon as possible.

* **void executeSync()**

	Executes the next function scheduled on the queue and returns.