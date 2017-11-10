Finder objects
==============

Finder objects are [sequences](sequence.md) that return the results of a search
in a given base sequence (which is often a collection object like Array or List).
The Finder sequence contain all elements that match the given search criteria.

Like all sequences, Finder object provide begin() and end() methods, which in turn
return [iterators](iterator.md) for iterating over the results.

Since they are sequences, Finder objects can be used with the range based for loop to iterate
over all matching elements (see example code below).

Finder objects have a few special properties:

No caching
----------

Finder objects do not cache the results. They work directly on the base sequence.

In particular that means that the begin() method always starts a fresh search.
If the base sequence changed after the Finder was created and then the Finder's begin()
method is called then the results will be up-to-date and reflect the current state of the
base sequence.

Iterators
---------

The Finder iterators iterate over the matching elements in the base sequence.
So for a Finder iterator called "it", `*it` will return a reference to the corresponding
element in the base sequence. Likewise `it->someField` can be used to directly access
fields or methods of the element.

The ++ operator of the Finder iterator will advance the iterator to the next matching
element in the base sequence.

Note that the Finder iterators are custom objects - they do not have the same type
as normal iterators of the base sequence. BUT they can be implictly converted to
base iterators. So you can pass a Finder iterator in most places where a normal
iterator of the base sequence is expected.

In addition, the Finder iterators also provide the method getBaseIterator(), which
returns a base sequence iterator that points to the same place in the base sequence
as the Finder iterator.

A base sequence iterator can also be assigned to a Finder iterator (using the = operator)
to move it to the specified place. If the assigned place does not point to a matching
element then the Finder will automatically advance itself to the next matching element.
This feature allows Finder iterators to be used to remove elements from the base sequence,
for example (see demonstration code below).

Finder iterator reference
-------------------------

In the following table "it" and "itA", "itB" are used as the names of Finder iterator object.
baseIt is used as the name fpr an iterator of the base sequence.


| Operation                         | Description
-------------------------------------------------------------
| BaseIterator                      | A typedef to the iterator type of the base sequence.
|                                   | This can be either Iterator or ConstIterator of the base sequence,
|                                   | depending on whether or not the finder was created by a const or non-const
|                                   | find method.
-------------------------------------------------------------
| `++it`                            | Advance the iterator to the next matching element
|                                   |
| `it++`                            | 
-------------------------------------------------------------
| `*it`                             | Returns a reference to the element the iterator points to
-------------------------------------------------------------
| `it->someMethodOrField`           | Directly accesses a field or method of the element that the iterator points to.
-------------------------------------------------------------
| `baseIt = it.getBaseIterator()`   | Returns an iterator object from the base sequence that points to
|                                   | the same place as `it`.
-------------------------------------------------------------
| `baseIt = it`                     | Implicit conversion to a base sequence iterator. Has the same effect
|                                   | as calling getBaseIterator.
-------------------------------------------------------------
| `it = baseIt`                     | Sets the finder iterator to the same position as the specified base iterator.
|                                   | If the base iterator does not point to an element that matches the search criteria
|                                   | of the Finder then it automatically advances itself to the next matching element.
-------------------------------------------------------------
| itA == itB                        | Returns true if the iterators point to the same place.
|                                   | The iterators do not have to be from the same search operation.
-------------------------------------------------------------
| itA != itB                        | Inverse of itA == itB. Returns true if the iterators do not point to the same place.
-------------------------------------------------------------
| it == baseIt                      | Returns true if the finder iterator points to the same place as the specified base iterator.
|                                   |
| baseIt == it                      |
-------------------------------------------------------------
| it != baseIt                      | Inverse of it==baseIt. Returns true if the iterators do not point to the same place.
|                                   |
| baseIt != it                      |
-------------------------------------------------------------



Example
-------

```
Array<String> myArray = ...;

// findAll returns a finder that iterates over all matching
// elements. In this case we search for all strings shorter than 10 character.
auto finder = myArray.findAll(
    [](const String& el)       
    {
        return (el.getLength() < 10);
    } );


// iterate over all matching elements
// with the range based for loop.
for( const String& element: finder )
{
    // in each iteration we will get one matching element from the search.
    ...
}

// manually access the elements using iterators.
// This loop will first access the matching elements and then
// remove them.
auto finderIterator = finder.begin();
while( finderIterator != finder.end() )
{
    // elements can be accessed with *
    String el = *finderIterator;

    // You can directly call methods of the elements with ->
    if( finderIterator->isEmpty() )
    {
        // in this example we remove empty strings.

        // You can use the finder iterator in places where a normal
        // iterator is expected. For example, to remove elements from the base sequence.    
        // Note that removing the element invalidate the iterator and we would not
        // be able to continue the loop and find more matches.
        // To fix that we can simply assign the base sequence iterator that removeAt returns
        // to our finderIterator.
        finderIterator = myArray.removeAt( finderIterator );

        // removeAt returns a base sequence iterator that points to the element following
        // the removed element. When a base iterator is assigned to a finder iterator,
        // the finder iterator will automatically check if the element at the new position matches the search
        // criteria. If not then the finder iterator will automatically advance itself to the next
        // matching element.
        // So that means that now finderIterator point to the next matching element after the one that was
        // removed. If no further matching elements exist then the finderIterator will equal
        // finder.end().
        // So we can simply continue the loop now. We do not advance the finderIterator
        // here because it already points to the next match after the element we removed.
    }
    else
    {
        // we do not remove non-empty strings. So we simply advance the iterator
        // to the next match with the ++ operator.

        ++finderIterator;
    }
}

```

