#ifndef BDN_Map_H_
#define BDN_Map_H_

#include <map>

#include <bdn/StdCollection.h>

namespace bdn
{

/** A container that stores key-value pairs. The key is used to access the value very efficiently.
        
    The Map class requires that there is a well defined ordering among the keys. Any ordering is fine,
    but Map must be able to determine if key A comes before key B in that ordering.
    This information is used to optimize the data structures and enable efficient access.

    By default the Map class uses the < operator on the keys to determine their ordering.
    This can be changed with the third template parameter (for example, if the keys are objects that
    do not support the < operator). If specified, the third template parameter must be the
    type of a compare function object. An instance of this compare function object type can be passed to the
    constructor of Map. If no compare object is passed to the constructor then one is automatically
    created.

    Keys A and B are considered to be equal if neither one is smaller than the other.
    I.e. Neither A < B nor B < A is true with "<" being the ordering of the elements in the set.
        
    The Map class is derived from std::map and is 100 % compatible with it.
    It can be used as a drop-in replacement.

    Map provides renamed aliases for some std::map methods and types to ensure that
    they match the naming conventions used otherwise in the framework. The new
    names are also intended to make using the class more intuitive for newcomers that may not be familiar with
    the C++ standard library.

    Note that Map is also derived from bdn::Base, so it can be used with smart pointers (see bdn::P).
    
    Like the C++ standard library collections, one can specify an "allocator" type as an optional
    third template argument. This is used when special custom memory management is needed.
    If you do not need that then you should ignore the ALLOCATOR template argument and leave it at the default. 

*/
template<
	typename KEYTYPE,
	typename VALTYPE,
	typename COMPAREFUNCTYPE = std::less<KEYTYPE>,
	class ALLOCATOR = std::allocator< std::pair<const KEYTYPE, VALTYPE> > >
class Map : public StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >
{
public:

    typedef typename StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::key_type      Key;
    typedef typename StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::mapped_type   Value;
    
    using typename StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::Element;   
    using typename StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::Size;
    using typename StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::Iterator;
    using typename StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::ConstIterator;
    using typename StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::ReverseIterator;
    using typename StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::ConstReverseIterator;

    Map()
        : Map( COMPAREFUNCTYPE() )
    {
    }

	explicit Map( const COMPAREFUNCTYPE& compareFunc, const ALLOCATOR& alloc = ALLOCATOR() )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( compareFunc, alloc )
    {
    }

    explicit Map( const ALLOCATOR& alloc )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( alloc )
    {
    }


    template< class InputIt >
    Map(    InputIt beginIt,
            InputIt endIt,
            const COMPAREFUNCTYPE& compareFunc = COMPAREFUNCTYPE(),
            const ALLOCATOR& alloc = ALLOCATOR() )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( beginIt, endIt, compareFunc, alloc )
    {
    }

    template< class InputIt >
    Map(    InputIt beginIt,
            InputIt endIt,
            const ALLOCATOR& alloc)
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( beginIt, endIt, COMPAREFUNCTYPE(), alloc)
    {
    }


    Map( const Map& other )
        : Map( static_cast<const std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>&>( other ) )
    {
    }

	
    Map( const std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>& other )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( other )
    {
    }


    Map( const Map& other, const ALLOCATOR& alloc )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( other, alloc )
    {
    }

    Map( const std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>& other, const ALLOCATOR& alloc )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( other, alloc )
    {
    }


    Map( Map&& other )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( std::move( static_cast<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>&&>(other) ) )
    {
    }

    Map( std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>&& other )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( std::move(other) )
    {
    }

    Map( Map&& other, const ALLOCATOR& alloc )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( std::move( static_cast<std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>&&>(other) ), alloc )
    {
    }

    Map( std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>&& other, const ALLOCATOR& alloc )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( std::move(other), alloc )
    {
    }



    Map(    std::initializer_list<Element> initList,
            const COMPAREFUNCTYPE& compFunc = COMPAREFUNCTYPE(),
            const ALLOCATOR& alloc = ALLOCATOR() )
        : StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >( initList, compFunc, alloc )
    {
    }

    Map(    std::initializer_list<Element> initList,
            const ALLOCATOR& alloc )
        : Map(initList, COMPAREFUNCTYPE(), alloc)
    {
    }


    /** Replaces the current contents of the map with copies of the elements from the specified other
        Map.
        
        Returns a reference to this Map object.
        */
    Map& operator=( const Map& other )
    {
        std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


    /** Replaces the current contents of the map with copies of the elements from the specified
        std::map object.
        
        Returns a reference to this Map object.
        */
    Map& operator=( const std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>& other )
    {
        std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=( other );
        return *this;
    }


     /** Replaces the current contents of the set with copies of the elements from the specified
        initializer list. This is called by the compiler if a  "= {...} " statement is used.

        Note that each element is a Key, Value pair (a std::pair<const Key, Value> object).
        
        For example:

        \code
        // a map object that maps integers to their textual representation (just an example)
        Map<int, String> myMap;

        // we now add three elements to the map. Each is a pair of an integer and a string.

        myMap = {   {1, "one"}
                    {17, "seventeen"}
                    {42, "fortytwo"}
                };

        \endcode

        Returns a reference to this Map object.
        */
    Map& operator=( std::initializer_list<Element> initList )
    {
        std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=(initList);
        return *this;
    }  




    /** Moves the data from the specified other Map object to this set, replacing any current contents in the process.
        The other Map object is invalidated by this operation.
        */
    Map& operator=( Map&& other )
        noexcept( ALLOCATOR::propagate_on_container_move_assignment::value || ALLOCATOR::is_always_equal::value )
    {
        std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=( std::move(other) );
        return *this;
    }
    

    /** Moves the data from the specified other Map object to this set, replacing any current contents in the process.
        The other Map object is invalidated by this operation.
        */
    Map& operator=( std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>&& other )
        noexcept( ALLOCATOR::propagate_on_container_move_assignment::value || ALLOCATOR::is_always_equal::value )
    {
        std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR>::operator=( std::move(other) );
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

        std::pair<Iterator, bool> result = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::insert( keyValuePair );

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
        std::pair<Iterator, bool> result = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::insert( std::move(keyValuePair) );

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
            std::pair<Iterator, bool> result = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::insert( *it );

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

        \begin
        Map<int, String> myMap;

        // we now add three elements to the map. Each is a pair of an integer and a string.
        myMap.addSequence( {    {1, "one"}
                                {17, "seventeen"}
                                {42, "fortytwo"}
                           }  );    

        */
    void addSequence( std::initializer_list<Element> initList )
    {
        // again, we cannot use the batch version of map::insert because of the overwrite semantics
        for( const Element& el: initList)
        {
            std::pair<Iterator, bool> result = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::insert( el );

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
        std::pair<iterator,bool> result = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::emplace( std::forward<Args>(args)... );

        if(!result.second)
        {
            // this is quite inefficient - but there is probably no other way. We must construct a temporary
            // pair to get the value.
            result.first->second = std::pair<const Key,Value>( std::forward<Args>(args)... ).second;
        }            

		return *result.first;
    }




	/** In general, this collection operation prepares the collection for a bigger insert operation.
        This function is provided for consistency with other collection types only - for bdn::Map this
        function has no effect and does nothing.
    */
    void prepareForSize(Size size)
    {
        // do nothing
    }


    /** Returns true if the map contains the specified key/value pair.

        This function does not just check that the key is in the map - it also
        checks the associated value.

        The parameter is a std::pair<const KEYTYPE, VALTYPE> object.

        Returns true if the key/value pair is in the map and false otherwise.
    */
    bool contains( const Element& el ) const
    {
        auto it = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::find(el.first);

        if(it == this->end())
            return false;
        else
            return (it->second == el.second);
    }


	class Finder
	{
	public:
		class ItWrapper_
		{
		public:
			ItWrapper_(Map& map, Iterator it )
				: _map(map)
				, _it(it)
			{
			}

			typename const Map::Element& operator*() const
			{
				return *_it;
			}

			typename const Map::Element& operator*() const
			{
				return *_it;
			}

			typename Map::Element* operator->() const
			{
				return &*_it;
			}

			typename Map::Element& operator*() const
			{
				return *_it;
			}

			bool operator==(const ItWrapper& other)
			{
				return (_it == other._it );
			}

			bool operator!=(const ItWrapper& other)
			{
				return (_it != other._it );
			}
			
			ItWrapper& operator++()
			{
				// the map can only have one matching element.
				// So there can only be two iterators: a potential
				// match iterator and end.
				// ++ on end is undefined, so we do not care about the result.
				// and ++ on a match iterator should set it to end.
				// So we can just set the iterator to end in all cases.
				_it = map.end();

				return *this;
			}

			ItWrapper operator++(int)
			{
				ItWrapper oldVal = *this;
				operator++();

				return oldVal;
			}

		private:
			Map&		_map;
			Iterator	_it;
		};

		ItWrapper_ begin()
		{
			return ItWrapper_(_map, _foundIt);
		}

		ItWrapper_ end()
		{
			return ItWrapper_(_map, _map.end() );
		}

	private:		
		Finder( const Map& map, const Element& toFind)
			: _map(map)
			, _foundIt( map->find(toFind) )
		{
		}

		Map&		_map;
		Iterator	_foundIt;

		friend class Map;
	};

	XXX
	Finder findElement(const Element& toFind)
	{
		return Finder(*this, toFind);
	}


	XXX
	Finder findByKey(const Key& keyToFind)
	{
		return Finder(*this, toFind);
	}


    /** Searches for the specified element in the set.
        The search uses the set's compare function to determine if a set element is equal to 
        the \c toFind parameter. The two elements are considered to be equal if neither
        one is "smaller" than the other.

        find is very fast in the Map class. Its complexity is logarithmic to the size of the set.
            
        Returns an iterator to the found element, or end() if no such element is found.
    */
	Iterator find( const Element& toFind )
	{
        auto it = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::find( toFind.first );

		if(it!=this->end() && it->second == toFind.second)
			return it;
		else
			return this->end();
	}


    /** Const version of find() - returns a read-only iterator.
    */
	ConstIterator find( const Element& toFind ) const
	{
		auto it = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::find( toFind.first );

		if(it!=this->end() && it->second == toFind.second)
			return it;
		else
			return this->end();
	}

	
    /** Searches for the first element for which the specified condition function returns true.        
        conditionFunc must take a collection element reference as its only parameter and return a boolean.

        Note that for the Map class findCondition() is a lot slower than find(). find can take advantage
        of the internal data structures of the set to find the element in logarithmic time. findCondition 
        on the other hand needs to check potentially all elements in the set (i.e. it has linear complexity).
            
        Returns an iterator to the found element, or end() if no such element is found.
    */
    template<class ConditionFuncType>
	Iterator findCondition(ConditionFuncType conditionFunc )
	{
        return std::find_if( this->begin(), this->end(), conditionFunc );
	}


    /** Const version of findCondition() - returns a read-only iterator.
    */
    template<class ConditionFuncType>
	ConstIterator findCondition(ConditionFuncType conditionFunc ) const
	{
        return std::find_if( this->begin(), this->end(), conditionFunc );
	}



    /** If the set contains the specified element, remove it. Does nothing if the element
        is not in the set.*/
    void findAndRemove(const Element& val)
    {
		auto it = find(val);
		if( it!=this->end() )
			StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::erase( it );
    }


    
    /** Removes all elements for which the specified function checkFunc returns true.
    
        checkFunc must be a function that takes a reference to a set element as its parameter
        and returns true if the element should be removed.
    */
    template<typename CheckFuncType>
    void findConditionAndRemove( CheckFuncType& checkFunc )
    {
        for(auto it = begin(); it!=end(); )
        {
            if( checkFunc(*it) )
                it = StdCollection< std::map<KEYTYPE, VALTYPE, COMPAREFUNCTYPE, ALLOCATOR> >::erase( it );
            else
                ++it;
        }
    }

        

};


}


#endif
