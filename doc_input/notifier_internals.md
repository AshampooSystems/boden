Notifier: Threading and subscription management  {#notifier_internals}
===============================================

This page discusses the decisions made for the Notifier class.


Posting notifications to the main thread
----------------------------------------

Notifier objects post all notifications to the main thread. So why are the subscribed functions not called directly?

The reason is that notifiers are intended to be used in any context where something interesting happens that
some other component of the app might want to know about. So notifications can be sent from any context in any thread,
with arbitrary mutexes and resources being locked at that point in time. That means that subscribed functions would not
only have to deal with the fact that they are called from an unknown thread, they also would have to be very careful
with the resources they access. In particular, locking any mutex or other resource could lead to deadlocks. In many 
cases the problems caused by an improper implementation of the subscribed function can be very hard to test for and
could cause rare sporadic bugs.

Posting all notifications to the main thread solves these issues. Subscribed functions are always called from
the main thread and they can be certain that no mutexes etc. are locked at the time of the call. That makes it
very straight forward and simple to implement these functions and the additional overhead for the application is
negligible for all but the most frequent notifications.

Admittedly, if tens of thousands of notifications per second happen in your app then the performance penalty of
posting these notifications might be inacceptable. In these cases the Notifier class is simply not the right choice
and a different kind of notification mechanism should be used.


Unsubscribe
-----------

You might be wondering why the INotifierSubControl::unsubscribe() method does not always guarantee that the unsubscribed
function will not be called after unsubscribe returns. It is only guaranteed for cases when unsubscribe is called from
the main thread. Admittedly, having this guarantee would make it easier to implement subscribed functions for cases when
a necessary resource is deleted. In particular, the lack of such a guarantee means that one cannot simply unsubscribe
a plain method in the destructor of a class (since the destructor could be called from a worker thread and the
notification method might still be called after unsubscribing).

The reasoning behind the implemented behaviour is pretty simple: a hard guarantee like the one mentioned above
would require a mutex to be locked whenever a subscribed function is called and also when functions are unsubscribed.
That in turn is a recipe for disaster - or rather a recipe for creating deadlocks. Consider the following case:

- Main thread:
    - Notifier calls subscribed function F
- Background thread:
    - locks mutex M
    - calls unsubscribe for F
    - unsubscribe would now have to wait until F finishes in the main thread
- Main thread:
    - F tries to lock M => This is a now deadlock. The main thread waits for M to be released by the background
      thread and the background thread waits for F to finish.

Like with many potential deadlocks, such cases can be very hard to test and debug, since the deadlock might only
occur when the functions are executed with a specific timing. It might never happen on any developer machine,
but it might happen on a customer machine that is particularly slow, has a different kind of CPU, has a slow hard drive,
or has any other kind of special property that could influence the timing.

To avoid such potential deadlocks, the programmers that implement the subscribed functions and the unsubscribe call
would have to be very careful and consider what resources the notification function accesses and in what context
unsubscribe might be called. The latter in particular can be especially difficult if unsubscribe is called in a
destructor of an object that is managed by smart pointers like \ref bdn::P. The object might be deleted in some distant
program component at some arbitrary time point in time that has little to do with the original component where the code
is implemented.

So, as always, Boden tries to avoid such potential for disaster whenever possible. The only way to do this is to NOT
hold a mutex when subscription functions are called. That makes implementing subscribed functions very easy,
but it also means that no hard unsubscribe guarantee for worker threads can be given.

So what can you do if you need a hard unsubscribe that is definitely finished at a certain point in the code?
The best advice is to evaluate if you *really* need it. If you want to unsubscribe because
a required object is deleted, consider simply keeping the object alive until the unsubscription has finished
(using \ref bdn::strongMethod() for your notification function, for example). 

