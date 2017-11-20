#ifndef BDN_HashMap_H_
#define BDN_HashMap_H_

#include <unordered_map>

#include <bdn/StdMapCollection.h>

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
class HashMap : public StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >
{
public:

    using typename StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::Allocator;
    using typename StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::Iterator;
    using typename StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::ConstIterator;
    using typename StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::Element;
    using typename StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::Key;
    using typename StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >::Value;

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

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
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

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
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

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initialBucketCount,
			hasher,
			EQUALITYCHECKERTYPE(),
			alloc)
	{
	}


	explicit HashMap( const Allocator& alloc )
		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( alloc )
	{
	}


	template< class InputIt >
	HashMap( InputIt beginIt, InputIt endIt)
		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( beginIt, endIt )
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

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
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

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
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

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
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
        : StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( other )
    {
    }


    HashMap( const HashMap& other, const ALLOCATOR& alloc )
        : StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( other, alloc )
    {
    }

    HashMap( const std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>& other, const ALLOCATOR& alloc )
        : StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >( other, alloc )
    {
    }


    HashMap( HashMap&& other )
        : StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			std::move( static_cast<std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&&>(other) ) )
    {
    }

    HashMap( std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&& other )
        : StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			std::move(other) )
    {
    }

    HashMap( HashMap&& other, const ALLOCATOR& alloc )
        : StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			std::move( static_cast<std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&&>(other) ), alloc )
    {
    }

    HashMap( std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR>&& other, const ALLOCATOR& alloc )
        : StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			std::move(other), alloc )
    {
    }



	HashMap( std::initializer_list< Element > initList )

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
			initList )
	{
	}

	HashMap(
		std::initializer_list< Element > initList,
		Size initialBucketCount,
		const HASHERTYPE& hasher = HASHERTYPE(),
		const EQUALITYCHECKERTYPE& equalityChecker = EQUALITYCHECKERTYPE(),
		const Allocator& allocator = Allocator() )

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
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

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
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

		: StdMapCollection< std::unordered_map<KEYTYPE, VALTYPE, HASHERTYPE, EQUALITYCHECKERTYPE, ALLOCATOR> >(
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
	

        

};


}


#endif
