Sequence
========

In the Boden framework, the term "sequence" is used to describe an object that provides
a sequence of elements of some type via begin() and end() methods. This concept is also an
essential part of the C++ standard library.

begin() and end() must each return an [iterator](iterator.md) object. These iterators can be used
to access the individual elements of the sequence.

begin() returns an iterator that points to the first element of the sequence.

end() returns an iterator that points to the position *just after* the last element of the sequence.

If the sequence is empty (it contains 0 elements) then the iterators returned by begin() and
end() are equal.

All collection classes in the standard C++ library and the Boden framework
conform to the sequence concept. The same is true for string classes and various other types and objects.

Sequences can be modifid and filtered with the utility functions in [bdn/sequenceUtil.h]

Note that sequences can also be used with the "range based for loop":

```
// in this example, "sequence" is a sequence object.
// The for loop will iterate over all elements in the sequence.
// "currEl" is the name of the current element inside the loop.
for( auto& currEl: sequence )
{
    ... do something with the current element
}
```



