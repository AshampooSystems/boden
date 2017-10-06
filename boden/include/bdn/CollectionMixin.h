#ifndef BDN_CollectionMixin_H_
#define BDN_CollectionMixin_H_

#include <vector>

namespace bdn
{

template<class BaseCollectionType >
class CollectionMixin : public BaseCollectionType
{
public:
    typedef BaseCollectionType::value_type ValueType;
    typedef BaseCollectionType::allocator_type AllocatorType;
    typedef BaseCollectionType::size_type Size;
    
    typedef BaseCollectionType::iterator Iterator;
    typedef BaseCollectionType::const_iterator ConstIterator;
    

    Array() noexcept( noexcept(Allocator()) )
     : std::vector( Allocator() )
    {
    }

    explicit Array( const Allocator& alloc ) noexcept
        : std::vector(alloc)
    {
    }

    Array( size_type count, const ValueType& value, const Allocator& alloc = Allocator() )
        : std::vector(count, value, alloc)
    {
    }

    explicit Array( size_type count, const Allocator& alloc = Allocator() )
        : std::vector(count, alloc)
    {
    }
    
    template< class InputIt >
    Array( InputIt beginIt, InputIt endIt, const Allocator& alloc = Allocator() )
        : std::vector(beginIt, endIt, alloc)
    {
    }

    Array( const Array& other )
        : std::vector(other)
    {
    }

    Array( const std::vector& other )
        : std::vector(other)
    {
    }

    Array( const vector& other, const Allocator& alloc )
        : std::vector(other, alloc)
    {
    }

    Array( Array&& other ) noexcept
        : std::vector( std::forward(other) )
    {
    }

    Array( std::vector&& other ) noexcept
        : std::vector( std::forward(other) )
    {
    }

    Array( Array&& other, const Allocator& alloc )
        : std::vector( std::forward(other), alloc )
    {
    }

    Array( std::vector&& other, const Allocator& alloc )
        : std::vector( std::forward(other), alloc )
    {
    }

    Array( std::initializer_list<ValueType> init, const Allocator& alloc = Allocator() )
        : std::vector( init, alloc )
    {
    }


    Allocator getAllocator() const
    {
        return std::vector::get_allocator();
    }


    Array& operator=( const std::vector& other )
    {
        std::vector::operator=( other );
        return *this;
    }

    Array& operator=( const Array&& other ) noexcept( Allocator::propagate_on_container_move_assignment::value || Allocator::is_always_equal::value )
    {
        std::vector::operator=( std::forward(other) );
        return *this;
    }

    Array& operator=( const std::vector&& other ) noexcept( Allocator::propagate_on_container_move_assignment::value || Allocator::is_always_equal::value )
    {
        std::vector::operator=( std::forward(other) );
        return *this;
    }

    Array& operator=( std::initializer_list<ValueType> initList )
    {
        std::vector::operator=(initList);
        return *this;
    }


    void assign( size_type count, const ValueType& value )
    {
        std::vector::assign(count, value);
    }
	
    template< class InputIt >
    void assign( InputIt beginIt, InputIt endIt )
    {
        std::vector::assign(beginIt, endIt);
    }

    void assign( std::initializer_list<T> initList )
    {
        std::vector::assign(initList);
    }


    ValueType& elementAt( size_type index )
    {
        return std::vector::at(index);
    }

    const ValueType& elementAt( size_type index ) const
    {
        return std::vector::at(index);
    }


    ValueType& at( size_type index )
    {
        return std::vector::at(index);
    }

    const ValueType& at( size_type index ) const
    {
        return std::vector::at(index);
    }


    ValueType& operator[]( size_type index )
    {
        return std::vector::at(index);
    }

    const ValueType& operator[]( size_type index ) const
    {
        return std::vector::at(index);
    }


    ValueType& firstElement()
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::firstElement called on empty array.");

        return std::vector::front();
    }

    const ValueType& firstElement() const
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::firstElement called on empty array.");

        return std::vector::front();
    }

    ValueType& front()
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::front called on empty array.");

        return std::vector::front();
    }

    const ValueType& front() const
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::front called on empty array.");

        return std::vector::front();
    }


    ValueType& lastElement()
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::lastElement called on empty array.");

        return std::vector::back();
    }

    const ValueType& lastElement() const
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::lastElement called on empty array.");

        return std::vector::back();
    }

    ValueType& back()
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::back called on empty array.");

        return std::vector::back();
    }

    const ValueType& back() const
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::back called on empty array.");

        return std::vector::back();
    }

    
    ValueType* data() noexcept
    {
        return std::vector::data();
    }
    
    const ValueType* data() const noexcept
    {
        return std::vector::data();
    }


    Iterator begin() noexcept
    {
        return std::vector::begin();
    }

    ConstIterator begin() const noexcept
    {
        return std::vector::begin();
    }

    ConstIterator constBegin() const noexcept
    {
        return std::vector::cbegin();
    }


    Iterator end() noexcept
    {
        return std::vector::end();
    }

    ConstIterator end() const noexcept
    {
        return std::vector::end();
    }

    ConstIterator constEnd() const noexcept
    {
        return std::vector::cend();
    }


    ReverseIterator reverseBegin() noexcept
	{
		return std::vector::rbegin();
	}

    ConstReverseIterator reverseBegin() const noexcept
	{
		return std::vector::rbegin();
	}

    ConstReverseIterator constReverseBegin() const noexcept
	{
		return std::vector::crbegin();
	}


    
    ReverseIterator reverseEnd() noexcept
	{
		return std::vector::rend();
	}

    ConstReverseIterator reverseEnd() const noexcept
	{
		return std::vector::rend();
	}

    ConstReverseIterator constReverseEnd() const noexcept
	{
		return std::vector::crend();
	}




    bool isEmpty() const noexcept
    {
        return std::vector::empty();
    }

    Size getSize() const noexcept
    {
        return std::vector::size();
    }

    Size getMaxSize() const noexcept
    {
        return std::vector::max_size();
    }
    
    void prepareForSize(Size elementCount)
    {
        std::vector::reserve(elementCount);
    }



    void clear() noexcept
    {
        std::vector::clear();
    }
    


    Iterator insert( ConstIterator pos, const ValueType& value)
    {
        return std::vector::insert(pos, value);
    }

    Iterator insert( ConstIterator pos, const ValueType&& value)
    {
        return std::vector::insert(pos, std::forward(value) );
    }


    Iterator insert( ConstIterator pos, Size count, const ValueType& value)
    {
        return std::vector::insert(pos, count, value);
    }

    template< class InputIt >
    Iterator insert( ConstIterator pos, InputIt first, InputIt last )
    {
        return std::vector::insert<InputIt>(pos, first, last);
    }

    Iterator insert( ConstIterator pos, std::initializer_list<ValueType> initList )
    {
        return std::vector::insert(pos, initList);
    }


    template< class... Args > 
    Iterator insertNew( ConstIterator pos, Args&&... args )
    {
        return std::vector::emplace(pos, args);
    }

    

    void add( const ValueType& value )
    {
        std::vector::push_back(value);
    }

    void add( ValueType&& value )
    {
        std::vector::push_back( std::forward(value) );
    }

    template< class... Args > 
    ValueType& addNew(Args&&... args)
    {
        emplace_back( std::forward<Args>(args)... );
        return back();
    }



    Iterator erase( ConstIterator pos )
    {
        return std::vector::erase(pos);
    }

    Iterator erase( ConstIterator beginIt, ConstIterator endIt )
    {
        return std::vector::erase(beginIt, endIt);
    }

    void removeLast()
    {
        if(isEmpty())
    		throw OutOfRangeError("Array::removeLast called on empty array.");

        std::vector::pop_back();
    }
    

    void setSize( size_type count )
    {
        std::vector::resize(count);
    }

    void setSize( size_type count, const ValueType& initValue )
    {
        std::vector::resize(count, initValue);
    }



    Iterator find( const ValueType& value )
    {
        return std::find(begin(), end(), value );
    }

    ConstIterator find( const ValueType& value ) const
    {
        return std::find(begin(), end(), value );
    }    

    template<class Predicate>
	Iterator findCondition(Predicate condition ) const
	{
        return std::find_if( begin(), end(), condition );
	}



    Iterator reverseFind( const ValueType& value )
    {
        ReverseIterator endIt = reverseEnd();
        ReverseIterator it = std::find(reverseBegin(), endIt );
        if(it==reverseEnd)
            return end();

        // it.base is the iterator of the element AFTER the one it points to
        return it.base()--;
    }

    ConstIterator reverseFind( const ValueType& value ) const
    {
        ConstReverseIterator endIt = reverseEnd();
        ConstReverseIterator it = std::find(reverseBegin(), endIt );
        if(it==reverseEnd)
            return end();

        // it.base is the iterator of the element AFTER the one it points to
        return it.base()--;
    }    

    template<class Predicate>
	Iterator reverseFindCondition(Predicate condition ) const
	{
        ConstReverseIterator

        return std::find_if( begin(), end(), condition );
	}



    Size indexOf( const ValueType& value ) const
    {
        const Iterator it = std::find(begin(), end(), value );

        if( it==end() )
            return noMatch;
        else
            return it-begin();
    }


    template<class Predicate>
	Size indexOfCondition(Predicate condition ) const
	{
        const Iterator it = std::find_if(begin(), end(), condition );

        if( it==end() )
            return noMatch;
        else
            return it-begin();
	}




    void sort()
    {
        std::sort( begin(), end() );
    }

    template<class ComesBeforeFuncType>
    void sort(ComesBeforeFuncType comesBefore )
    {
        std::sort( begin(), end(), comesBefore );
    }

    void stableSort()
    {
        std::stable_sort( begin(), end() );
    }

    template<class ComesBeforeFuncType>
    void stableSort( ComesBeforeFuncType comesBefore )
    {
        std::stable_sort( begin(), end(), comesBefore );
    }
};


}


#endif
