Iterator
========

An "iterator" is an object that can be used to iterate over a sequence of elements.

The iterator concept is actually defined in the C++ standard. Boden uses the same
concept - it is only documented here for convenience.

In many ways, an iterator is similar to a pointer. In fact, normal C++ pointers also
conform to the iterator concept.

An iterator points to an element in an underlying sequence elements. It can be moved
to other elements, and it can be used to access the element it points to.

A special case are so called "end iterators", which point to the place just after the last element
in the underlying sequence. These end iterators are used as "end markers" only - for example, so that
one knows where to stop a loop that iterates over the sequence using iterators.
Since end iterators do not point usually point to a to a real element, it is not allowed to use any operations
that access the element the iterator points to.

There are several types of iterators. Some iterators only allow advancing forwards over the underlying
sequence of elements. If going back to previous elements is supported then the iterator is called a "bidirectional
iterator". Some iterators even go one step further and also support advancing multiple steps at once in any
direction -- these are called "random access iterators".

Iterators support the following operations (here "it", "itA" and "itB" represent iterator objects and "n" represents
an integer number):

|Operation          | Description
----------------------------------------------------------------
| *it               | Returns a reference to the element that the iterator currently points to.
|                   | Depending on the type of iterator, this might return a read-only or a writable reference.
|                   | So for some kinds of iterators this can be used to change the element that the iterator
|                   | points to.
|                   |
|                   | Example:
|                   |
|                   | ```
|                   | // read access
|                   | ElementType currentElement = *it;
|                   | 
|                   | // write access (not supported by all iterator types)
|                   | *it = someElement;
|                   | ```
----------------------------------------------------------------
| ++it              | Advances the iterator to the next element.
|                   | 
|                   | `++it` returns a reference to the iterator
|                   | itself, so this can be chained with another operation.
| it++              | 
|                   | `it++` returns a copy of the original iterator, before it was advanced.
|                   | 
|                   | It is recommended to use ++it instead of `it++`, because it faster and simpler 
|                   | to understand.
|                   | 
|                   | Example:
|                   | 
|                   | ```
|                   | ++it; // simply advances to the next element
|                   | 
|                   | Element myElement = *(++it);  // advances to the next element and then accesses it with
|                   |                               // the * operation described above.
|                   | ```
-----------------------------------------------------------------
| it->              | This can be used to directly access a field or method of the element.
|                   | This works the same way as the -> operator with normal C++ pointers.
| (not supported by | 
| all iterators)    | This is supported by the majority of iterators. But some write-only iterator types
|                   | might not support it.
|                   | 
|                   | Also, this member access operator only makes sense when the element that the iterator
|                   | points to is an object of some kind (class, struct, etc.). If the iterator points
|                   | to a simple type (for example, an int), then there is no member that could be accessed with
|                   | the -> operator.
|                   | 
|                   | Example:
|                   | 
|                   | ```
|                   | it->someFunctionOfTheElement();   // directly calls a function of the element the iterator points to.
|                   | ```
-----------------------------------------------------------------
| == and !=         | Iterators can be compared with other iterators with the `==` and `!=` operators.
|                   | Two iterators compare equal when they point to the same place in the sequence.
|                   | 
|                   | Example:
|                   | 
|                   | ```
|                   | // we loop until the iterator compares equal to the end iterator of the sequence
|                   | while( it != sequence.end() )
|                   | {
|                   |   // do something with the iterator
|                   |   it->doSomething();
|                   | 
|                   |   // advance to the next element
|                   |   ++it;   
|                   | }
|                   | ```
-----------------------------------------------------------------
| itA = itB         | Iterators can be set to point to the same place as another iterator.
|                   | Given two iterators a and b then `a = b` changes iterator a to point to the same place as b.
|                   | 
-----------------------------------------------------------------
| ItType itA( itB ) | Iterator types (here ItType is a stand in for the type of the iterator objects)
|                   | provide a copy constructor. Here, the new iterator itA is initialized to point to the same place as itB.
-----------------------------------------------------------------
| ItType it         | Default construction. Most iterator objects can be constructed without parameters. They will be invalid and
| (not supported by | not point to any particular place, so it is necessary to initialize them afterwards with the = operator.
| all iterators)    | before they are used.
|                   | 
|                   | This is supported by most iterator types, but not by all of them.
-----------------------------------------------------------------
| --it              | The inverse of `++it` and `it++`. Moves the iterator back to the previous element.
|                   | 
| it--              | This operation is only supported by some iterators.
|                   | If it is supported then the iterator is called a bidirectional iterator.
| (only             | 
| bidirectional     | `--it` returns a reference to the iterator itself, so this can be chained with another operation.
| iterators)        | 
|                   | `it--` returns a copy of the original iterator, before it was advanced.
|                   | 
|                   | It is recommended to use `--it` instead of `it--`, because it faster and simpler 
|                   | to understand.
|                   | 
-----------------------------------------------------------------
| it += n           | Similar to moving the iterator by 1 via `++it` and `--it`, some
|                   | iterators also support moving the iterator multiple steps in
| it -= n           | one direction, by "adding" a number n to it.
|                   | 
| (random access    | Iterators that support this are called random access iterators.
| iterators only)   | For these, moving the iterator multiple steps is usually not slower
|                   | than moving it one step. I.e. the operation takes the same time, no
|                   | matter how big the number n is.
|                   | 
|                   | 
|                   | Example:
|                   | 
|                   | ```
|                   | // move iterator 6 steps backwards
|                   | it -= 6;
|                   | ```
-----------------------------------------------------------------
| it + n, n + it    | Creates a copy of the iterator and moves it n steps forwards or backwards.
|                   | 
| it - n, n - it    | Similar to the `+=` and `-=` operators, these operations are only supported
|                   | by random access iterators.
| (random access    | 
| iterators only)   | These are useful when you do not want to modify the original iterator.
|                   | 
|                   | Example:
|                   | 
|                   | ```
|                   | // copy of the iterator, advance the copy 3 steps ahead and
|                   | // then pass the result to the function someFunc.
|                   | someFunc( it + 3 );
|                   | ```
-----------------------------------------------------------------
| itA < itB         | Compares two iterators and checks which one points to an earlier / later
|                   | place in the underlying sequence.
| itA <= itB        | 
|                   | These operations are only supports by random access iterators.
| itA > itB         | 
|                   | 
| itA >= itB        | 
|                   | 
| (random access    | 
| iterators only)   | 
-----------------------------------------------------------------
| itA - itB         | Returns the number of steps between two iterators.
|                   | 
| (random access    | For example, if itA is 3 steps ahead from itB then 
| iterators only)   | `itA - itB` returns 3.
|                   | 
|                   | These operations are only supported by random access iterators.
-----------------------------------------------------------------
| it[ n ]           | Returns a reference to the element n steps ahead of the
|                   | place that the iterator currently points to. n can also be negative
| (random access    | to access elements before the iterator position.
| iterators only)   | 
|                   | These operations are only supported by random access iterators.
-----------------------------------------------------------------

