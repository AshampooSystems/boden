Collections
==============

Collections are containers for other elements (arrays, lists, maps, etc).

Boden provides several collection classes. All of them derive from standard C++ container classes like
std::vector or std::list. The Boden collection classes are 100% compatible with the standard classes,
meaning that they can be used as drop-in replacements. See below for a discussion of the advantages
that the Boden collections have over standard collections.


Operations supported by all collection classes
----------------------------------------------

Note that Boden collections (like the standard C++ collections) are also [sequence.md](sequences).

Boden collections all support the following functionality:

| Method / type                                                         | Description
--------------------------------------------------
| `Element`                                                             | type of the elements contained in the collection
--------------------------------------------------
| `Iterator`                                                            | type of the iterator objects of the collection
--------------------------------------------------
| `ConstIterator`                                                       | type of the read-only iterator objects of the collection
--------------------------------------------------
| `ElementFinder`                                                       | type of the [finder.md](finder object) returned by findAll().
--------------------------------------------------
| `template<typename MatchFunc> FuncFinder`                             | type of the [finder.md](finder object) returned by findAllCustom().
|                                                                       | This is a template type that depends on the match function passed to findAllCustom.
-------------------------------------------------------------
| `Size`                                                                | integer type that is used for element counts and indices.
|                                                                       | This is usually size_t.
--------------------------------------------------
| `Iterator begin()`                                                    | iterate over the elements of the collection. See [sequence.md](sequence).
|                                                                       |
| `Iterator end()`                                                      |
|                                                                       |
| `ConstIterator begin() const`                                         |
|                                                                       |
| `ConstIterator end() const`                                           |
|                                                                       |
| `ConstIterator constBegin() const`                                    |
|                                                                       |
| `ConstIterator constEnd() const`                                      |                        
-------------------------------------------------------------
| `Iterator removeAt( ConstIterator position )`                         | Removes the element at the position indicated by the iterator.
|                                                                       | Returns an iterator to the following element, or end() if there are
|                                                                       | no more elements.
-------------------------------------------------------------
| `Iterator removeSection( ConstIterator beginIt, ConstIterator endIt)` | Removes all elements between the iterators beginIt and endIt.
|                                                                       | The removed elements include the element pointed to by beginIt,
|                                                                       | do *not* include the element pointed to by endIt.
|                                                                       | Returns an iterator to the element following the removed section,
|                                                                       | or end() if no more elements follow.
-------------------------------------------------------------
| `void findAndRemove( const Element& toRemove )`                       | Removes all elements from the collection that compare equal to `toRemove`.
-------------------------------------------------------------
| `template<typename MatchFuncType>`                                    | Removes all elements for which the specified match function returns true.
| `void findConditionAndRemove( MatchFuncType matchFunc )`              | The match function can be any function that takes an Element reference as its parameter
|                                                                       | and returns either true or false.
-------------------------------------------------------------
| `void clear()`                                                        | Remove all elements from the collection
-------------------------------------------------------------
| `bool isEmpty() const`                                                | Returns true if the collection is empty
-------------------------------------------------------------
| `Size getSize() const`                                                | Returns the number of elements in the collection
-------------------------------------------------------------
| `Size getMaxSize() const`                                             | The maximum number of elements supported by the collection, assuming sufficient
|                                                                       | available memory. This limit usually depends on the size of addressable memory
|                                                                       | and the point at which internal counters etc. overflow.
|                                                                       | In practice this is always a huge number and normal programs need not be concerned
|                                                                       | with it.
-------------------------------------------------------------
| `Collection& operator=(const Collection& sourceCollection)`           | Assigns a copy of the sourceCollection's contents to the target collection (replacing
|                                                                       | any previous content the target collection might have had). Returns a reference to the
|                                                                       | target collection.
|                                                                       | sourceCollection can also be the corresponding standard C++ collection (e.g. std::list for bdn::List).
-------------------------------------------------------------
| `void add( const Element& el)`                                        | Adds the specified element to the collection. The position at which it is inserted depends on
|                                                                       | the collection type. For example, bdn::Array and bdn::List add it at the end. Other collections
|                                                                       | automatically determine the position based on the internal data organization (like bdn::Map).
-------------------------------------------------------------
| `template< class InputIt >`                                           |  Adds all elements between the two iterators beginIt and endIt to the collection.
| `void addSequence( InputIt beginIt, InputIt endIt )`                  |  The iterators can come from any collection or [sequence.md](sequence).
---------------------------------------------------------------------
| `void addSequence( std::initializer_list<Element> initList )`         | Adds the elements from the specified initializer list to the collection.
|                                                                       | This allows elements to be added with the {} notation. For example:
|                                                                       | `addSequence( {element1, element2, element3} )`
-----------------------------------------------------------------------
| `template< typename... Args >`                                        | Constructs a new element with the specified arguments as constructor parameters
| `[const] Element& addNew( Args&&... args )`                           | and adds it to the collection. Returns a reference to the added element.
|                                                                       | For most collection types the returned reference is a read/write reference.
|                                                                       | But for some collections (like bdn::Set) it is a const reference, since elements
|                                                                       | cannot be changed after being added to the set.
|                                                                       | 
|                                                                       | addNew is useful mainly for performance optimization. It can be used when a brand new element
|                                                                       | is to be added to the collection. Then addNew constructs the new element directly constructed
|                                                                       | at the proper place inside the collection's internal data structures. If one were to use the normal
|                                                                       | add() then the element would first be constructed as a temporary object and then copied or moved
|                                                                       | into the collection. This second copy/move step can be avoided with addNew.
-----------------------------------------------------------------------
| `void prepareForSize( Size size )`                                    | Prepares the collection for a big add operation. prepareForSize will not modify the
|                                                                       | collection's current size (number of elements). It will only optimize the internal data structures
|                                                                       | so that the collection can efficiently grow up to the specified target size.
|                                                                       | Calling prepareForSize is always optional. It only exists for performance optimization.
-----------------------------------------------------------------------
| `bool contains( const Element& el ) const`                            | Returns true if the collection contains the specified element.
-----------------------------------------------------------------------
| `ElementFinder findAll( const Element& el) const`                     | Searches for all elements that compare equal to the specified element.
|                                                                       | Returns a [finder.md](finder object) with the results.
-----------------------------------------------------------------------
| `template<class MatchFuncType>`                                           | Searches for all elements for which the specified match function returns true.
| `FuncFinder<MatchFuncType> findAllCustom( MatchFuncType matchFunction )`  | The match function can be any function that takes an Element reference as its parameter
|                                                                           | and returns either true or false.
|                                                                           | findAllCustom returns a [finder.md](finder object) with the results.
-----------------------------------------------------------------------



Why not use the standard C++ containers?
----------------------------------------

There are several reasons for why Boden provides its own extensions
instead of using the standard classes directly:

- Improved consistency among different collection classes: Boden provides several additional
  functions that work the same on all collections
  (like add, addNew, removeAt, findAll, etc.). This makes it easier to write generic code that works with
  multiple collections.

- Centralizing common functionality: the Boden collections try to provide all commonly used functionality
  as methods. For example, bdn::Array and bdn::List provide sort() methods, instead of relying on external
  global functions. This makes it easier to find what you need, if you are not familiar with the standard
  library.

- Improved names: the functionality of the Boden collections is named in accordance with the Boden
  naming standards. In particular, the names try to be more precise in describing what the methods
  actually do, making it easier to understand code written by others.

- Adherence to naming conventions. This is a minor point, but the Boden collections provide some aliases
  for functionality inherited from the standard library collections. These aliases ensure that the functionality is also
  available under names that comply with the Boden naming conventions. Having consistent naming throughout
  the framework improves the ability to use it intuitively.
