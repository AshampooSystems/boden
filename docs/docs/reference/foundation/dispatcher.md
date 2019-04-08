path: tree/master/framework/ui/include/bdn/
source: Dispatcher.h

# Dispatcher

???

## Declaration

```C++
class Dispatcher
```

## Types

* **using Clock = std::chrono::steady_clock**
* **using TimePoint = Clock::time_point**
* **using Duration = Clock::duration**
* **using SteppedDuration = std::chrono::duration<long long, std::deci\>**

## Enums

* **Priority**

	```c++
        idle = -100,
        normal = 0,
	```

## Enqueue

* **virtual void enqueue(std::function<void()\> function, Priority priority = Priority::normal) = 0**

	Enqueues a `function` on the dispatchers thread.

* **virtual void enqueueDelayed(Dispatcher::Duration delay, std::function<void()> function, Priority priority = Priority::normal) = 0**

	Enqueues a `function`to run on the dispatchers thread after `delay` nanoseconds have passed.

## Timer

* **virtual void createTimer(Dispatcher::Duration interval, std::function<bool()\> function)**

	Creates a timer that will run `function` repeatedly on the dispatchers thread every `interval` nanoseconds until it returns false.