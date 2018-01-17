#ifndef BDN_DefaultProperty_H_
#define BDN_DefaultProperty_H_


#include <bdn/Property.h>
#include <bdn/func.h>
#include <bdn/DefaultNotifier.h>

namespace bdn
{

/** The default implementation of a Property.

	The initial value of the property is the default-constructed ValType.
	For integers that is 0.

	*/
template<class ValType>
class DefaultProperty : public Property<ValType>
{
public:
	DefaultProperty(ValType value = ValType() )
	{
        _pImpl = newObj<Impl>(value);
	}
    
	ValType get() const override
	{
        return _pImpl->get();
	}

	void set(const ValType& val) override
	{
        _pImpl->set(val);
	}


	Property<ValType>& operator=(const ValType& val) override
    {
        set(val);
        
        return *this;
    }

	Property<ValType>& operator=(const ReadProperty<ValType>& prop) override
    {
        set(prop.get());
        
        return *this;
    }

	INotifier< P<const IValueAccessor<ValType>> >& onChange() const override
	{
        return _pImpl->onChange();
	}
    
    void bind(const ReadProperty<ValType>& sourceProperty) override
	{
        _pImpl->bind(sourceProperty);
    }

private:    

    /** We need the actual implementation to be allocated with new. At the same time,
        we want ptoperties to be very convenient to use - they should be a drop-in replacement
        for normal values. It is definitely more convenient to be able to allocate DefaultProperty
        objects on the stack or as direct members.

        To work around this problem we implement all the functionality in an internal Impl object.
        DefaultProperty is only a wrapper for a pointer to this object.
        */
    class Impl : public Base, BDN_IMPLEMENTS IValueAccessor<ValType>
    {
    public:
        Impl(ValType value )
	    {
            _pOnChange = newObj< DefaultNotifier< P<const IValueAccessor<ValType>> > >();
		    _value = value;
	    }

        ValType get() const override
	    {
		    MutexLock lock(_mutex);

		    return _value;
	    }

	    void set(const ValType& val)
	    {   
		    bool changed = false;

		    {
			    MutexLock lock(_mutex);

			    if(_value!=val)
			    {
				    _value = val;
				    changed = true;
			    }
		    }

		    if(changed)
			    _pOnChange->postNotification(this);
	    }


	    INotifier< P<const IValueAccessor<ValType>> >& onChange() const
	    {
		    return *_pOnChange;
	    }
    
        void bind(const ReadProperty<ValType>& sourceProperty)
	    {
            sourceProperty.onChange() += weakMethod(this, &Impl::bindSourceChanged);
        
            bindSourceChanged( &sourceProperty );
        }
        
    private:
        void bindSourceChanged( P<const IValueAccessor<ValType>> pValue )
        {
            set( pValue->get() );
        }
    

	    mutable Mutex					_mutex;
	    ValType							_value;
    
        mutable P< DefaultNotifier< P<const IValueAccessor<ValType>> >	> _pOnChange;
    };


    P<Impl> _pImpl;
};




}

#endif
