#ifndef BDN_Nullable_H_
#define BDN_Nullable_H_

namespace bdn
{


/** Encapsulates an arbitrary object or value and adds the possibility
    to set it to "null".
    
    This is useful if you want to pass objects around by value (instead
    of by pointer or reference), but you still want the option to pass
    an "invalid" or "null" value.
    
    Note that Nullable throws an exception if an attempt is made to
    access the value of an object that is marked as "null".
    
    Example:
    
    \code
    
    void myFunc( Nullable<int> valOrNull )
    {
        if(valOrNull.isNull())
            ... do something for the null case
        else
        {
            int val = valOrNull;
            
            ... do something with the integer value.
        }
    }
    
    \endcode
    */
template<typename ValueType>
class Nullable
{
public:
    Nullable()
    : _null(true)
    {
        _null = true;
    }
    
    Nullable(std::nullptr_t)
    : _null(true)
    {
    }

    
    Nullable(const ValueType& value)
    : _null(false)
    , _value(value)
    {
    }
    
    Nullable(ValueType&& value)
    : _null(false)
    , _value(value)
    {
    }
    
    
    Nullable( const Nullable<ValueType>& o)
    : _null( o._null )
    , _value( o._value )
    {
    }
    
    Nullable( Nullable<ValueType>&& o)
    : _null( o._null )
    , _value( std::move(o._value) )
    {
        o._null = true;
    }
    
    
    
    bool operator==(std::nullptr_t) const
    {
        return _null;
    }
    
    bool operator!=(std::nullptr_t) const
    {
        return !_null;
    }


    bool operator==(const ValueType& v) const
    {
        return (!_null && _value==v);
    }
    
    bool operator!=(const ValueType& v) const
    {
        return !operator==(v);
    }
    
    
    bool operator==(const Nullable<ValueType>& o) const
    {
        return (_null==o._null && (_null || _value==o._value) );
    }
    
    bool operator!=(const Nullable<ValueType>& o) const
    {
        return !operator==(o);
    }
    
    
    Nullable& operator=(std::nullptr_t)
    {
        _null = true;
        _value = ValueType();
        
        return *this;
    }
    
    
    Nullable& operator=(const ValueType& value)
    {
        _null = false;
        _value = value;
        
        return *this;
    }
    
    Nullable& operator=(ValueType&& value)
    {
        _null = false;
        _value = value;
        
        return *this;
    }
    
    Nullable& operator=(const Nullable& o)
    {
        _null = o._null;
        _value = o._value;
        
        return *this;
    }
    
    Nullable& operator=(Nullable&& o)
    {
        _null = o._null;
        _value = std::move(o._value);
        
        o._null = true;
        
        return *this;
    }

    
    
    /** Returns true if the value is "null".*/
    bool isNull() const
    {
        return _null;
    }
    
    
    /** Returns the value if it is not null.
        If it is null then an exception is thrown.*/
    const ValueType& get() const
    {
        if(_null)
            programmingError("An attempt was made to access the value of a bdn::Nullable object that is null.");
        
        return _value;
    }
    
    operator const ValueType&() const
    {
        return get();
    }
    
private:
    bool        _null;
    ValueType   _value;
};
    
}


#endif

