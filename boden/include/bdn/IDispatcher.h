#ifndef BDN_IDispatcher_H_
#define BDN_IDispatcher_H_

#include <map>

namespace bdn
{

/** Interface for dispatchers. Dispatchers handle scheduling of small tasks
	with different priorities.

	They are often used to schedule something to be executed by a certain thread
	or at certain times.
	*/
class IDispatcher : BDN_IMPLEMENTS IBase
{
public:

	enum class Priority
	{
		idle	= -200,		
		low		= -100,
		normal	= 0,
		high	= 100
	};


	/** Schedules the specified function to be executed
		with the specified priority.
		
		Lower priority items are only executed when there are no higher priority items
		available to be executed.

		enqueue() can be called from any thread.

		*/
	virtual void enqueue(
		std::function< void() > func,
		Priority priority = Priority::normal )=0;


	/** Schedules the specified function to be executed after
		the specified number of seconds.
		
		Note that the seconds parameter is a double, so you can specify something
		like 0.25 to wait for 250 ms, for example.

		Other tasks can be executed during the wait time (event events with lower priority).

		Also see schedule().
		*/
	virtual void enqueueInSeconds(
		double seconds,
		std::function< void() > func,
		Priority priority = Priority::normal )=0;


	/** Creates a timer that calls the specified function regularly with the
		specified time interval.

		Note that many implementations have a lower limit for the timer intervals (usually around 10 milliseconds,
		i.e. 0.01 seconds). If the specified interval is less than that then the timer will be called with the
		limit interval.

		The specified timer function must return a bool. If it is true then the timer continues and the
		function will be called again. If it returns false then the timer is destroyed.

		Note that if there is already a call for this timer pending and the interval elapses again
		then the second call is not enqueued. In other words: if the timer is held up by higher
		priority work then there will be at most one event waiting in the queue and you will not get
		a quick succession of calls when the queue empties.
		*/
	void createTimer(
		double intervalSeconds,
		std::function< bool() > func );

	

};

}

#endif
