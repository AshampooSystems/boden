#ifndef BDN_StdMapCollection_H_
#define BDN_StdMapCollection_H_

#include <bdn/StdCollection.h>
#include <bdn/SequenceFilter.h>

namespace bdn
{



/** Base class for key->value mapping containers that are based on a standard C++ library container.

*/
template<
	class BASE_COLLECTION_TYPE >
class StdMapCollection : public StdCollection< BASE_COLLECTION_TYPE >
{
public:

	using Key = typename StdCollection< BASE_COLLECTION_TYPE >::key_type;
	using Value = typename StdCollection< BASE_COLLECTION_TYPE >::mapped_type;
    
    using typename StdCollection< BASE_COLLECTION_TYPE >::Element;   
    using typename StdCollection< BASE_COLLECTION_TYPE >::Size;
    using typename StdCollection< BASE_COLLECTION_TYPE >::Iterator;
    using typename StdCollection< BASE_COLLECTION_TYPE >::ConstIterator;
    
	
	template<typename... ARGS>
    StdMapCollection(ARGS&&... args)
        : StdCollection< BASE_COLLECTION_TYPE >( std::forward< ARGS >(args)... )
    {
    }
	    
    
    /** Adds the specified element to the map. The element is a key value pair, i.e.
        a std::pair<const KEYTYPE, VALUETYPE> object.

        Note that there is also a version of add() that takes the key and value as separate parameters.
        
        If the map already has an entry for the key then the associated value is overwritten
        with the new value.
        */
    void add( const std::pair<const Key, Value>& keyValuePair )
    {
        // note that this implementation might be slightly faster than (*this)[key] = value, since for new elements
        // no initial default-constructed element needs to be constructed. Instead insert can directly construct the new
        // element.

        std::pair<Iterator, bool> result = StdCollection< BASE_COLLECTION_TYPE >::insert( keyValuePair );

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
    void add( std::pair<const Key, Value>&& keyValuePair )
    {
        std::pair<Iterator, bool> result = StdCollection< BASE_COLLECTION_TYPE >::insert( std::move(keyValuePair) );

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
            std::pair<Iterator, bool> result = StdCollection< BASE_COLLECTION_TYPE >::insert( *it );

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
            std::pair<Iterator, bool> result = StdCollection< BASE_COLLECTION_TYPE >::insert( el );

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

		Map< int, String > myMap;

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
            std::pair<Iterator, bool> result = StdCollection< BASE_COLLECTION_TYPE >::insert( el );

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
    template< class... ARGS > 
    Element& addNew( ARGS&&... args )
    {
        std::pair<iterator,bool> result = StdCollection< BASE_COLLECTION_TYPE >::emplace( std::forward<ARGS>(args)... );

        if(!result.second)
        {
            // this is quite inefficient - but there is probably no other way. We must construct a temporary
            // pair to get the value.
            result.first->second = std::pair<const Key,Value>( std::forward<ARGS>(args)... ).second;
        }            

		return *result.first;
    }




	/** In general, this collection operation prepares the collection for a bigger insert operation.
        This function is provided for consistency with other collection types only - for map collections this
        function has no effect and does nothing.
    */
    void prepareForSize(Size size)
    {
        // do nothing
    }


    /** Returns true if the map contains the specified key/value pair.

        This function does not just check that the key is in the map - it also
        checks the associated value. The parameter is a std::pair<const KEYTYPE, VALTYPE> object.

		Note that there is also an overload of contains() that checks only if the key is in the map.

        Returns true if the key/value pair is in the map and false otherwise.
    */
    bool contains( const Element& el ) const
    {
        auto it = StdCollection< BASE_COLLECTION_TYPE >::find(el.first);

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

        // this is a template function so that it works with both normal and const iterators
		// and set references
		template<class COLL, class ITERATOR>
		void operator() (COLL& map, ITERATOR& it)
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

        // this is a template function so that it works with both normal and const iterators
		// and set references
		template<class COLL, class ITERATOR>
		void operator() (COLL& map, ITERATOR& it)
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

	template<typename MATCH_FUNC_TYPE>
    class FuncMatcher_
    {
    public:
        FuncMatcher_( MATCH_FUNC_TYPE matchFunc )
            : _matchFunc( matchFunc )
        {
        }

        // this is a template function so that it works with both normal and const iterators
		// and set references
		template<class COLL, class ITERATOR>
		void operator() (COLL& map, ITERATOR& it)
        {
            // note that the "it" parameter is NEVER equal to end() when we are called.
            // That also means that we are never called for empty maps.

            while( ! _matchFunc(it) )
            {
                ++it;
                if( it==map.end() )
                    break;
            }
        }    

	private:
		MATCH_FUNC_TYPE _matchFunc;
    };

	
	// note that in C++11 there is a theoretical problem here (though not one in practice).
	// The advanceAfterRemoval method of the SequenceFilter Iterators assumes that the order
	// of the elements does not change when an element is removed. In C++11 this is not
	// guaranteed for std::unordered_map. In C++14 it IS guaranteed.
	// Luckily, it seems that all known implementations in C++11 do NOT reorder the elements
	// when one is removed. So in practice this is not a problem. And since C++14 guarantees
	// that the ordering remains the same, this will also not become a problem in the future.
	// Also see
	// https://stackoverflow.com/questions/25047241/c11-is-it-safe-to-remove-individual-elements-from-stdunordered-map-while-it

	using KeyFinder = SequenceFilter< StdMapCollection, KeyMatcher_>;
	using ConstKeyFinder = SequenceFilter<const StdMapCollection, KeyMatcher_>;

	using ElementFinder = SequenceFilter<StdMapCollection, ElementMatcher_>;
	using ConstElementFinder = SequenceFilter<const StdMapCollection, ElementMatcher_>;

	template<typename MATCH_FUNC_TYPE>
	using CustomFinder = SequenceFilter<StdMapCollection, FuncMatcher_<MATCH_FUNC_TYPE> >;

	template<typename MATCH_FUNC_TYPE>
	using ConstCustomFinder = SequenceFilter<const StdMapCollection, FuncMatcher_<MATCH_FUNC_TYPE> >;



	/** Searches for the specified key in the map and returns a finder object.
		A finder object is a \ref sequence.md "sequence" with all matching elements.
		
		Since Map does not allow multiple entries with the same key the returned finder
		can only return either 1 or 0 objects.
		*/
	KeyFinder findAll(const Key& keyToFind)
	{
		return KeyFinder(*this, KeyMatcher_(keyToFind) );
	}

	/** Searches for the specified key in the map and returns a finder object.
		A finder object is a \ref sequence.md "sequence" with all matching elements.
		
		Since Map does not allow multiple entries with the same key the returned finder
		can only return either 1 or 0 objects.
		*/
	ConstKeyFinder findAll(const Key& keyToFind) const
	{
		return ConstKeyFinder(*this, KeyMatcher_(keyToFind) );
	}

	/** Searches for the specified element (key/value pair) in the map and returns a \ref finder.md "finder object"
		with the results.
		
		Since HashMap does not allow multiple entries with the same key the returned finder
		can only return either 1 or 0 objects.
		*/
	ElementFinder findAll(const Element& elToFind)
	{
        return ElementFinder(*this, ElementMatcher_(elToFind) );
	}


	/** Searches for the specified element (key/value pair) in the map and returns a \ref finder.md "finder object"
		with the results.
		
		Since HashMap does not allow multiple entries with the same key the returned finder
		can only return either 1 or 0 objects.
		*/
	ConstElementFinder findAll(const Element& elToFind) const
	{
        return ConstElementFinder(*this, ElementMatcher_(elToFind) );
	}


	/** Searches for all elements for which the specified match function returns true.
		
		The match function can be any function that takes a Map iterator as its parameter
		and returns true if the element at the corresponding position should be in the find results.

		findAllCustom returns a \ref finder.md "finder object" with the results.
		*/
	template<class MatchFuncType>
	CustomFinder<MatchFuncType> findAllCustom( MatchFuncType matchFunction )
	{
		return CustomFinder<MatchFuncType>(*this, FuncMatcher_<MatchFuncType>(matchFunction) );
	}

	/** Searches for all elements for which the specified match function returns true.
		
		The match function can be any function that takes a Map iterator as its parameter
		and returns true if the element at the corresponding position should be in the find results.

		findAllCustom returns a \ref finder.md "finder object" with the results.
		*/
	template<class MatchFuncType>
	ConstCustomFinder<MatchFuncType> findAllCustom( MatchFuncType matchFunction ) const
	{
		return ConstCustomFinder<MatchFuncType>(*this, FuncMatcher_<MatchFuncType>(matchFunction) );
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
        Iterator it = StdCollection< BASE_COLLECTION_TYPE >::find( toFind.first );
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
        return StdCollection< BASE_COLLECTION_TYPE >::find( toFind );
	}


    /** Const version of find() - returns a read-only iterator.
    */
	ConstIterator find( const Element& toFind ) const
	{
        ConstIterator it = StdCollection< BASE_COLLECTION_TYPE >::find( toFind.first );
		if( it!=this->end() && it->second==toFind.second )
			return it;
		else 
			return this->end();
	}

	/** Const version of find() - returns a read-only iterator.
    */
	ConstIterator find( const Key& toFind ) const
	{
        return StdCollection< BASE_COLLECTION_TYPE >::find( toFind );		
	}




    /** If the map contains the specified key/value pair, remove it.

		This only removes an element from the map if key AND its associated value match the specified one.
		Note that there is also an overload of findAndRemove() that only searches for the key.
	
		Does nothing if the element is not found in the map.*/
    void findAndRemove(const Element& val)
    {
		auto it = find(val);
		if( it!=this->end() )
			StdCollection< BASE_COLLECTION_TYPE >::erase( it );
    }

	/** If the map contains the specified key, remove the corresponding entry.
		
		Does nothing if no element with the key is found in the map.*/
    void findAndRemove(const Key& key)
    {
		StdCollection< BASE_COLLECTION_TYPE >::erase( key );
    }



    
    /** Removes all elements for which the specified function matchFunc returns true.
    
        The match function can be any function that takes a Map iterator as its parameter
		and returns true if the element at the corresponding position should be removed.
    */
    template<typename MATCH_FUNC_TYPE>
    void findCustomAndRemove( MATCH_FUNC_TYPE& matchFunc )
    {
        for(auto it = begin(); it!=end(); )
        {
            if( matchFunc(it) )
                it = StdCollection< BASE_COLLECTION_TYPE >::erase( it );
            else
                ++it;
        }
    }




	



	/** Returns a locale independent string representation of the map.*/
	String toString() const
	{
		if(isEmpty())
			return "{}";
		else
		{
			String s = "{ ";

			bool first = true;
			for(auto& el: *this)
			{
				if(!first)
					s += ",\n  ";
				s += bdn::toString(el.first)+": "+bdn::toString(el.second);
				first = false;
			}

			s += " }";

			return s;
		}
	}

};


}


#endif
