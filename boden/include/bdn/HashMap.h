#ifndef BDN_HashMap_H_
#define BDN_HashMap_H_

#include <unordered_map>

#include <bdn/StdCollection.h>
#include <bdn/SequenceFilter.h>

namespace bdn
{



/** A container that stores key-value pairs. The key can be used to access the value very efficiently.

	Data organization
	-----------------

	The HashMap class is very similar to the bdn::Map class. The main difference is how the elements are stored
	internally.

	HashMap organizes elements based on their so-called "hash value". The hash value is a number
	that is calculated based in the element's key using a configurable function (called the "hash function").

	Iteration order
	---------------

	Iterators of the HashMap class return the elements in an implementation dependent order (see begin()).
	Also, the order of all elements may completely change when elements are added to the HashMap.
	So, you should not assume anything regarding the order in which the iterators return the HashMap element.


	When to use HashMap
	-------------------
	
	HashMap should only be used if you can provide a good hash function for the key, with a low probability
	that two keys will have the same hash value. If the hash function is good and fast then accessing entries in
	the HashMap is also very fast - often faster than with bdn::HashMap. However, there are also disadvantages.
	HashMaps waste memory if they end up having less elements than expected. The performance characteristics are
	also quite complicated - they depend on the number of entries in the collection and the distribution of hash values.

	bdn::Map provides more consistent performance, with less room for degraded corner cases. And bdn::Map is also a little simpler to use, since
	providing a "< operator" for the keys is often easier than writing a good hash function.

	In general, it is recommended to default to use bdn::Map instead of bdn::HashMap unless you have a good reason
	to prefer HashMap.	

	Hash function
	-------------

	By default the HashMap class uses std::hash() as the hash function.
	std::hash is a template struct that whose objects are callable like a function.
	std::hash has specializations for all the simple types (integers, floating point
	numbers, pointers, char types and bool). If std::hash does not have a specialization for your HashMap's key type,
	then you have two options:

	1) provide your own specialization for std::hash for your key type

	2) or specify a custom hash function for your HashMap. The hash function can be configured as a template parameter
	   of the HashMap class.

	A hash function should take a reference to the HashMap's key type as its parameter and return a size_t value.
	A Hash function must have the following properties:

	- it must never throw an exception
	- if two keys are equal then the hash function must return the same hash value for them
	- the probability that the hash function returns the same hash value for two different keys should be as small as possible

	It is allowed for the hash function to return the same hash value for different keys.
	BUT you should be aware that if this happens too often in practice then this can severely impact the performance
	of the HashMap.

	See below for examples (including one how to provide a specialization of std::hash).

	Equality function
	-----------------
	
	In addition to the hash function, HashMap also needs a way to detect when two keys are equal.
	By default it uses the normal == operator for that. You can also provide a custom equality check
	function as a template parameter, if your keys do not support == or you want custom equality handling.

	Template parameters
	-------------------

	The HashMap class has the following template parameters:

	- KEYTYPE is the type of the key for the elements
	- VALUETYPE is the type of the values that are associated with the keys
	- HASHERTYPE is the type of the hasher that the HashMap should use. By default this is std::hash.
	- EQUALITYCHECKERTYPE is the type of the object that the HashMap uses to check if two keys are equal.
	  By default this uses std::equal_to<KeyType>, which means that the == operator is used (unless a custom specialization
	  for std::equal_to is provided).
	- ALLOCATOR is the type of the allocator the HashMap uses to manage its memory. By default this uses std::allocator.

	Examples
	--------
	
	The following example creates a HashMap that uses ints as keys and Strings as value
	Note that we do not need to worry about the hash function, since the default
	std::hash already supports int.

	\code

	HashMap<int, String> map;

	// store a key / value pair (7, "hello") in the map
	map[7] = "hello";

	\endcode

	The next example demonstrates how to overload std::hash for a custom key type.

	\code

	// as a key we use MyKey here, which has two numbers a and b as its members.
	struct MyKey
	{
		int a;
		int b;

		// note that the key also needs to have a == operator if we do not
		// provide a custom equality check function as a HashMap template parameter
		bool operator==(const MyKey& o) const
		{
			return (a==o.a && b==o.b);
		}
	};
	
	namespace std
	{
		// this is a specialization for the std::hash template for MyKey keys
		template <>
		struct hash< MyKey >
		{
			size_t operator()(const MyKey& key) const
			{
				// we combine a and b with XOR to get a single hash value
				// for the key.
				return key.a ^ key.b;
			}
		};
	}

	// now we can use MyKey as the key for our HashMap.
	// Note that we do not need to provide the HASHERTYPE template parameter, since we have
	// extended std::hash so that it can deal with MyKey objects.

	HashMap< MyKey, String> myMap;

	// add an entry to the map
	myMap[ MyKey{1,2} ] = "hello";

	\endcode

	And the last example shows how to create a map with a custom hash function and comparison function specified
	as template parameters.

	\code

	// as a key we use MyKey here, which has two numbers a and b as its members.
	struct MyKey
	{
		int a;
		int b;
	};

	
	class MyKeyHasher
	{
	public:
		size_t operator(const MyKey& l)
		{
			return key.a ^ key.b;
		}
	};


	class MyKeyEqualityChecker
	{
	public:
		MyKeyEqualityChecker()
		{
		}

		bool operator(const MyKey& l, const MyKey& r)
		{
			return ( l.a == r.a && l.b == r.b);
		}
	};

				
	// now we can create the HashMap.
	// Note that we pass MyKeyHasher as the type of the hasher
	// and MyKeyEqualityChecker as the type of the equality checker.

	HashMap< MyKey, String, MyKeyHasher, MyKeyEqualityChecker> myMap;
		
	// myMap has now automatically created instances of MyKeyHasher and MyKeyEqualityChecker.
	// If the hasher or checker object needs parameters to work then we can also pass
	// a specific MyKeyHasher and/or MyKeyEqualityChecker instance to the constructor of HashMap
	// like this:

	MyKeyHasher			 hasher;
	MyKeyEqualityChecker equalityChecker;
	
	// here you would configure the hasher and equalityChecker objects
	// ...

	// Then we pass them to the HashMap constructor.

	HashMap< MyKey, String, MyKeyHasher, MyKeyEqualityChecker> myMap2( hasher, equalityChecker);

	\endcode

	Buckets and load factor
	-----------------------
		
	HashMap organizes all elements into "buckets", based on their hash value. The hash map works best when each bucket has
	at most one element in it.

	The "load factor" is the average number of elements in each bucket: numberOfElements / bucketCount

	When the load factor reaches a certain threshold then the number of buckets are increased automatically and
	all elements are redistributed into the new buckets.

	This load factor threshold is called "maxLoadFactor" and it can be configured (see getMaxLoadFactor(), setMaxLoadFactor()).
	The default is 1 - i.e. the bucket count is increased when the number of elements in the map exceeds the bucket count.

	By default the map will automatically choose a "reasonable" initial bucket count. You can also override the default by passing
	the desired initial bucket count to the HashMap constructor.


	Base classes / std::unordered_map compatibility
	--------------------------------	

    The HashMap class is derived from std::unordered_map and is 100 % compatible with it.
    It can be used as a drop-in replacement.

    HashMap provides renamed aliases for some std::unordered_map methods and types to ensure that
    they match the naming conventions used otherwise in the framework. The new
    names are also intended to make using the class more intuitive for newcomers that may not be familiar with
    the C++ standard library.

    Note that HashMap is also derived from bdn::Base, so it can be used with smart pointers (see bdn::P).

	Allocator
	---------
    
    Like the C++ standard library collections, one can specify an "allocator" type as an optional
    third template argument. This is used when special custom memory management is needed.
    If you do not need that then you should ignore the ALLOCATOR template argument and leave it at the default. 

*/
template<
	typename KEYTYPE,
	typename VALTYPE,
	typename HASHERTYPE = std::hash<KEYTYPE>,
	typename EQUALITYCHECKERTYPE = std::equal_to<KEYTYPE>,
	class ALLOCATOR = std::allocator< std::pair<const KEYTYPE, VALTYPE> > >
class HashMap : public StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >
{
public:

    typedef typename StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::key_type      Key;
    typedef typename StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::mapped_type   Value;
    
    using typename StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::Element;   
    using typename StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::Size;
    using typename StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::Iterator;
    using typename StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::ConstIterator;


	HashMap()
	{
	}
	

	/** \param initialBucketCount the initial number of buckets that the map will start with. See class description
			for more information.
			*/
	explicit HashMap(
		Size initialBucketCount,
		const HASHERTYPE& hasher = HASHERTYPE(),
		const EQUALITYCHECKERTYPE& equalityChecker = EQUALITYCHECKERTYPE(),
		const Allocator& alloc = Allocator() )

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initialBucketCount,
			hasher,
			equalityChecker,
			alloc)
	{
	}

	/** \param initialBucketCount the initial number of buckets that the map will start with. See class description
			for more information.
			*/
	HashMap(
		Size initialBucketCount,
		const Allocator& alloc )

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initialBucketCount,
			HASHERTYPE(),
			EQUALITYCHECKERTYPE(),
			alloc)
	{
	}

	/** \param initialBucketCount the initial number of buckets that the map will start with. See class description
			for more information.
			*/
	HashMap(
		Size initialBucketCount,
		const HASHERTYPE& hasher,
		const Allocator& alloc )

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initialBucketCount,
			hasher,
			EQUALITYCHECKERTYPE(),
			alloc)
	{
	}


	explicit HashMap( const Allocator& alloc )
		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( alloc )
	{
	}


	template< class InputIt >
	HashMap( InputIt beginIt, InputIt endIt)
		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( beginIt, endIt )
	{
	}


	/** \param initialBucketCount the initial number of buckets that the map will start with. See class description
			for more information.
			*/
	template< class InputIt >
	HashMap(
		InputIt beginIt,
		InputIt endIt,
		Size initialBucketCount,
		const HASHERTYPE& hasher = HASHERTYPE(),
        const EQUALITYCHECKERTYPE& equalityChecker = EQUALITYCHECKERTYPE(),
        const Allocator& alloc = Allocator() )

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			beginIt,
			endIt,
			initialBucketCount,
			hasher,
			equalityChecker )
	{
	}

	template< class InputIt >
	HashMap(
		InputIt beginIt,
		InputIt endIt,
		Size initialBucketCount,
		const Allocator& alloc )

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			beginIt,
			endIt,
			initialBucketCount,
			alloc )
	{
	}


	template< class InputIt >
	HashMap(
		InputIt beginIt,
		InputIt endIt,
        Size initialBucketCount,
        const HASHERTYPE& hasher,
        const Allocator& alloc ) 

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			beginIt,
			endIt,
			initialBucketCount,
			hasher,
			EQUALITYCHECKERTYPE(),
			alloc )
	{
	}


    HashMap( const HashMap& other )
        : HashMap( static_cast<const std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&>( other ) )
    {
    }

	
    HashMap( const std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>& other )
        : StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( other )
    {
    }


    HashMap( const HashMap& other, const ALLOCATOR& alloc )
        : StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( other, alloc )
    {
    }

    HashMap( const std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>& other, const ALLOCATOR& alloc )
        : StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( other, alloc )
    {
    }


    HashMap( HashMap&& other )
        : StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			std::move( static_cast<std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&&>(other) ) )
    {
    }

    HashMap( std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&& other )
        : StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			std::move(other) )
    {
    }

    HashMap( HashMap&& other, const ALLOCATOR& alloc )
        : StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			std::move( static_cast<std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&&>(other) ), alloc )
    {
    }

    HashMap( std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&& other, const ALLOCATOR& alloc )
        : StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			std::move(other), alloc )
    {
    }



	HashMap( std::initializer_list< Element > initList )

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initList )
	{
	}

	HashMap(
		std::initializer_list< Element > initList,
		Size initialBucketCount,
		const HASHERTYPE& hasher = HASHERTYPE(),
		const EQUALITYCHECKERTYPE& equalityChecker = EQUALITYCHECKERTYPE(),
		const Allocator& allocator = Allocator() )

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initList,
			initialBucketCount,
			hasher,
			equalityChecker,
			allocator )
	{
	}

	HashMap(
		std::initializer_list< Element > initList,
        Size initialBucketCount,
		const Allocator& allocator ) 

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initList,
			initialBucketCount,
			HASHERTYPE(),
			EQUALITYCHECKERTYPE(),
			allocator )
	{
	}

	HashMap(
		std::initializer_list< Element > initList,
		Size initialBucketCount,
		const HASHERTYPE& hasher,
		const Allocator& allocator ) 

		: StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initList,
			initialBucketCount,
			hasher,
			EQUALITYCHECKERTYPE(),
			allocator )
	{
	}




    /** Replaces the current contents of the map with copies of the elements from the specified other
        HashMap.
        
        Returns a reference to this HashMap object.
        */
    HashMap& operator=( const HashMap& other )
    {
        std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


    /** Replaces the current contents of the map with copies of the elements from the specified
        std::unordered_map object.
        
        Returns a reference to this HashMap object.
        */
    HashMap& operator=( const std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>& other )
    {
        std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


     /** Replaces the current contents of the set with copies of the elements from the specified
        initializer list. This is called by the compiler if a  "= {...} " statement is used.

        Note that each element is a Key, Value pair (a std::pair<const Key, Value> object).
        
        For example:

        \code
        // a map object that maps integers to their textual representation (just an example)
        HashMap<int, String> myMap;

        // we now add three elements to the map. Each is a pair of an integer and a string.

        myMap = {   {1, "one"}
                    {17, "seventeen"}
                    {42, "fortytwo"}
                };

        \endcode

        Returns a reference to this HashMap object.
        */
    HashMap& operator=( std::initializer_list<Element> initList )
    {
        std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>::operator=(initList);
        return *this;
    }  




    /** Moves the data from the specified other HashMap object to this set, replacing any current contents in the process.
        The other HashMap object is invalidated by this operation.
        */
    HashMap& operator=( HashMap&& other )
        noexcept( std::allocator_traits<Allocator>::is_always_equal::value
				&& std::is_nothrow_move_assignable<HASHERTYPE>::value
				&& std::is_nothrow_move_assignable<EQUALITYCHECKERTYPE>::value )
    {
        std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }
    

    /** Moves the data from the specified other HashMap object to this set, replacing any current contents in the process.
        The other HashMap object is invalidated by this operation.
        */
    HashMap& operator=( std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&& other )
        noexcept( std::allocator_traits<Allocator>::is_always_equal::value
				&& std::is_nothrow_move_assignable<HASHERTYPE>::value
				&& std::is_nothrow_move_assignable<EQUALITYCHECKERTYPE>::value )
    {
        std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }

    
    
    /** Adds the specified element to the map. The element is a key value pair, i.e.
        a std::pair<const KEYTYPE, VALUETYPE> object.

        Note that there is also a version of add() that takes the key and value as separate parameters.
        
        If the map already has an entry for the key then the associated value is overwritten
        with the new value.
        */
    void add( const std::pair<const KEYTYPE, VALTYPE>& keyValuePair )
    {
        // note that this implementation might be slightly faster than (*this)[key] = value, since for new elements
        // no initial default-constructed element needs to be constructed. Instead insert can directly construct the new
        // element.

        std::pair<Iterator, bool> result = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::insert( keyValuePair );

        if(!result.second)
        {
            // element already existed in the map. Overwrite its value
            result.first->second = keyValuePair.second;
        }
    }


	/** Like add(), but instead of the new element being a copy of the specified
        element, the C++ move semantics are used to move the element data from the parameter
        \c el to the new collection element.
        */
    void add( std::pair<const KEYTYPE, VALTYPE>&& keyValuePair )
    {
        std::pair<Iterator, bool> result = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::insert( std::move(keyValuePair) );

        if(!result.second)
        {
            // element already existed in the map. Overwrite its value
            result.first->second = std::move(keyValuePair.second);
        }
    }

     
    /** Adds the specified key value pair to the map. 

        If the map already has an entry for the key then the associated value is overwritten
        with the new value.
        */
    void add( const Key& key, const Value& value )
    {
        // we have to use the [] iterator here to get exactly the semantics we want.
        // In C++17 we could use insert_or_assign instead.
        // But in C++11 we only have the option to do this. And all other insert or emplace
        // operations do not overwrite if the element is already in the collection.
        (*this)[key] = value;
    }

       

    /** Adds the elements from the specified [beginIt ... endIt)
        iterator range to the set.
        endIt points to the location just *after* the last element to add.

        Each element must be a key value pair, i.e. a std::pair<const KEYTYPE, VALUETYPE> object.

        If the map already has an entry for one or more of the new element keys then the associated value is overwritten
        with the new value.

        The beginIt and endIt iterators must not refer to the target map. They can be from an arbitrary other
        collection -- also from a collection of a different type. The only condition is that the element type of the
        source collection must be compatible to the element type of the target set (i.e. it must behave like a std::pair with
        the key and value type).
        */
    template< class InputIt >
    void addSequence( InputIt beginIt, InputIt endIt )
    {
        // we cannot use the iterator insert here, unfortunately, since that does not overwrite existing entries.
        // So we have to add the elements one by one. This should not make much of a difference, though, since
        // map implementations do not have a good way to optimize batch insertions with random keys.
        for( auto it = beginIt; it!=endIt; ++it)
        {
            std::pair<Iterator, bool> result = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::insert( *it );

            if(!result.second)
            {
                // element already existed in the map. Overwrite its value
                result.first->second = it->second;
            }
        }
    }


    /** Adds the elements from the specified initializer list to the collection.

        Each element must be a key value pair, i.e. a std::pair<const KEYTYPE, VALUETYPE> object.

        If the map already has an entry for one or more of the new element keys then the associated value is overwritten
        with the new value.

        This version of addSequence can be used to add multiple elements with the {...} notation. For example:

        \code
        HashMap<int, String> myMap;

        // we now add three elements to the map. Each is a pair of an integer and a string.
        myMap.addSequence( {    {1, "one"}
                                {17, "seventeen"}
                                {42, "fortytwo"}
                           }  );    

		\endcode

        */
    void addSequence( std::initializer_list<Element> initList )
    {
        // again, we cannot use the batch version of map::insert because of the overwrite semantics
        for( const Element& el: initList)
        {
            std::pair<Iterator, bool> result = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::insert( el );

            if(!result.second)
            {
                // element already existed in the map. Overwrite its value
                result.first->second = el.second;
            }
        }
    }

	/** Adds the elements from the specified source \ref sequence.md "sequence" to the collection.
		
		Since all collections are also sequences, this can be used to copy all elements from
		any other collection of any type, as long as it has a compatible element type.

        Each element must be a key value pair, i.e. a std::pair<const KEYTYPE, VALUETYPE> object.

        If the map already has an entry for one or more of the new element keys then the associated value is overwritten
        with the new value.

		\code

		HashMap< int, String > myMap;

		List< std::pair<int, String > > sourceList(
			{   {1, "one"}
                {17, "seventeen"}
                {42, "fortytwo"}
            } );


		// add all elements from sourceList to myMap
		myMap.addSequence( sourceList );
		
		\endcode
        */
	template<class SequenceType>
    void addSequence( const SequenceType& sequence )
    {
        for( const Element& el: sequence)
        {
            std::pair<Iterator, bool> result = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::insert( el );

            if(!result.second)
            {
                // element already existed in the map. Overwrite its value
                result.first->second = el.second;
            }
        }
    }


		
    /** Constructs a new element and adds it to the set, if it not yet in the set.
        The arguments passed to addNew are passed on to the constructor of the
        newly constructed element.

        Note that elements are key-value pairs (std::pair<const Key, Value> objects). So the
        arguments are passed to a std::pair constructor. Usually this means that
        one passes exactly two arguments: one for the key and one for the value.

        If the map already has an entry for the key then its value is overwritten.

        This function can be used instead of add() when one wants to add a newly
        created value more efficiently. With add() the new value would initially be constructed
        as a temporary variable and then a copy or move operation would be made 
        to bring it into the collection. addNew constructs the new element directly inside
        the collection, avoiding any copying or moving.

        Note that addNew is only more efficient than add() if the key is not yet in the
        map. If the entry is overwritten then addNew() may actually perform worse than add().

        Returns a reference to the map element.
        */
    template< class... Args > 
    Element& addNew( Args&&... args )
    {
        std::pair<iterator,bool> result = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::emplace( std::forward<Args>(args)... );

        if(!result.second)
        {
            // this is quite inefficient - but there is probably no other way. We must construct a temporary
            // pair to get the value.
            result.first->second = std::pair<const Key,Value>( std::forward<Args>(args)... ).second;
        }            

		return *result.first;
    }




	/** Prepares the collection so that it can efficiently grow. This function
		does not change the size of the collection - it only optimizes internal data structures in
		preparation.

		For HashMap, this may increase the bucket count, so that the HashMap will not exceed the maximum
		load factor (see getMaxLoadFactor()) when it reaches the specified size.
    */
    void prepareForSize(Size size)
    {
		this->reserve(size);
    }


	/** Returns the HashMap's maximum load factor. See the class description for more information.*/
	float getMaxLoadFactor() const noexcept
	{
		return this->max_load_factor();
	}

	/** Sets the hash map's maximum load factor. See the class description for more information.*/
	void setMaxLoadFactor(float maxFactor)
	{
		this->max_load_factor(maxFactor);
	}


	/** Returns the current number of buckets in the map (see class description for more information).*/
	Size getBucketCount() const
	{
		return this->bucket_count();
	}

	/** Changes the number of buckets of the hash map. See the class description for more information.

		Note that this rebuilds the internal data structures, so it should be used sparingly.

		The function will have no effect if the specified bucket count is smaller or equal to the current bucket count.
	*/
	void increaseBucketCountTo(Size bucketCount)
	{
		if(bucketCount > this->bucket_count() )
			this->rehash(bucketCount);
	}
	

    /** Returns true if the map contains the specified key/value pair.

        This function does not just check that the key is in the map - it also
        checks the associated value. The parameter is a std::pair<const KEYTYPE, VALTYPE> object.

		Note that there is also an overload of contains() that checks only if the key is in the map.

        Returns true if the key/value pair is in the map and false otherwise.
    */
    bool contains( const Element& el ) const
    {
        auto it = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::find(el.first);

        if(it == this->end())
            return false;
        else
            return (it->second == el.second);
    }


	/** Returns true if the map contains an entry with the specified key.
	
        Returns true if the key is in the map and false otherwise.
    */
    bool contains( const Key& key ) const
    {
		return (count(key) != 0);
    }


    class KeyMatcher_
    {
    public:
        KeyMatcher_(const Key& key)
            : _key( key )
        {
        }

        void operator() (HashMap& map, Iterator& it)
        {
            // note that the "it" parameter is NEVER equal to end() when we are called.
            // That also means that we are never called for empty maps.

            if( it==map.begin() )
                it = map.find( _key );
            else
                it = map.end();
        }

    private:
        Key _key;
    };


    class ElementMatcher_
    {
    public:
        ElementMatcher_(const Element& element)
            : _element( element )
        {
        }

        void operator() (HashMap& map, Iterator& it)
        {
            // note that the "it" parameter is NEVER equal to end() when we are called.
            // That also means that we are never called for empty maps.

            if( it==map.begin() )
            {
                it = map.find( _element.first );
                if( it != map.end() )
                {
                    // found a matching key. But is the associated value also equal? 

                    if( it->second != _element.second )
                        it = map.end();
                }
            }
            else
                it = map.end();
        }

    private:
        Element _element;
    };

	template<typename MatchFuncType>
    class FuncMatcher_
    {
    public:
        FuncMatcher_( MatchFuncType matchFunc )
            : _matchFunc( matchFunc )
        {
        }

        void operator() (HashMap& map, Iterator& it)
        {
            // note that the "it" parameter is NEVER equal to end() when we are called.
            // That also means that we are never called for empty maps.

            while( ! _matchFunc(*it) )
            {
                ++it;
                if( it==map.end() )
                    break;
            }
        }    

	private:
		MatchFuncType _matchFunc;
    };


	using KeyFinder = SequenceFilter<HashMap, KeyMatcher_>;
	using ElementFinder = SequenceFilter<HashMap, ElementMatcher_>;

	template<typename MatchFuncType>
	using CustomFinder = SequenceFilter<HashMap, FuncMatcher_<MatchFuncType> >;



	/** Searches for the specified key in the map and returns a finder object.
		A finder object is a \ref sequence.md "sequence" with all matching elements.
		
		Since HashMap does not allow multiple entries with the same key the returned finder
		can only return either 1 or 0 objects.
		*/
	KeyFinder findAll(const Key& keyToFind)
	{
		return KeyFinder(*this, KeyMatcher_(keyToFind) );
	}

	ElementFinder findAll(const Element& elToFind)
	{
        return ElementFinder(*this, ElementMatcher_(elToFind) );
	}


	/** Searches for all elements for which the specified match function returns true.
		The match function can be any function that takes an Element reference as its parameter
		and returns either true or false.

		findAllCustom returns a \ref finder.md "finder object" with the results.
		*/
	template<class MatchFuncType>
	CustomFinder<MatchFuncType> findAllCustom( MatchFuncType matchFunction )
	{
		return CustomFinder<MatchFuncType>(*this, FuncMatcher_<MatchFuncType>(matchFunction) );
	}

	

	


    /** Searches for the specified key/value pair in the map.

		This function does not just search for the key. If the key is in the map then
		it also checks if the value associated with the key matches the one from the specified
		element \c toFind.

		Note that there is also an overload of find() that only takes a Key object as its parameter.
		That version only for the key.
		
        find is quite fast: its complexity is logarithmic to the size of the map.
            
        Returns an iterator to the found element, or end() if no such element is found.
    */
	Iterator find( const Element& toFind )
	{
        Iterator it = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::find( toFind.first );
		if( it!=this->end() && it->second==toFind.second )
			return it;
		else 
			return this->end();
	}

	/** Searches for the specified key in the map.
			
        find is quite fast: its complexity is logarithmic to the size of the map.
            
        Returns an iterator to the element with the specified key, if one is found, or end() if no such element is found.
    */
	Iterator find( const Key& toFind )
	{
        return StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::find( toFind );
	}


    /** Const version of find() - returns a read-only iterator.
    */
	ConstIterator find( const Element& toFind ) const
	{
        ConstIterator it = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::find( toFind.first );
		if( it!=this->end() && it->second==toFind.second )
			return it;
		else 
			return this->end();
	}

	/** Const version of find() - returns a read-only iterator.
    */
	ConstIterator find( const Key& toFind ) const
	{
        return StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::find( toFind.first );		
	}


	/** Searches for the first element (key/value pair) for which the specified match function returns true.        
        matchFunc must take a collection element reference (a std::pair<Key,Value> object)
		as its only parameter and return a boolean.

        Note that for the HashMap class findCustom() is a lot slower than find(). find can take advantage
        of the internal data structures of the map to find the element in logarithmic time. findCustom
        on the other hand needs to check potentially all elements in the map (i.e. it has linear complexity).
            
        Returns an iterator to the found element, or end() if no such element is found.
    */
    template<typename MatchFuncType>
	Iterator findCustom( MatchFuncType matchFunc )
	{
        return std::find_if( this->begin(), this->end(), matchFunc );
	}


    /** Const version of findCustom() - returns a read-only iterator.
    */
    template<typename MatchFuncType>
	ConstIterator findCustom( MatchFuncType matchFunc ) const
	{
        return std::find_if( this->begin(), this->end(), matchFunc );
	}



    /** If the map contains the specified key/value pair, remove it.

		This only removes an element from the map if key AND its associated value match the specified one.
		Note that there is also an overload of findAndRemove() that only searches for the key.
	
		Does nothing if the element is not found in the map.*/
    void findAndRemove(const Element& val)
    {
		auto it = find(val);
		if( it!=this->end() )
			StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::erase( it );
    }

	/** If the map contains the specified key, remove the corresponding entry.
		
		Does nothing if no element with the key is found in the map.*/
    void findAndRemove(const Key& key)
    {
		StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::erase( key );
    }



    
    /** Removes all elements for which the specified function matchFunc returns true.
    
        matchFunc must be a function that takes a reference to a collection element as its parameter
        and returns true if the element should be removed.
    */
    template<typename MatchFuncType>
    void findCustomAndRemove( MatchFuncType& matchFunc )
    {
        for(auto it = begin(); it!=end(); )
        {
            if( matchFunc(*it) )
                it = StdCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::erase( it );
            else
                ++it;
        }
    }

        

};


}


#endif
