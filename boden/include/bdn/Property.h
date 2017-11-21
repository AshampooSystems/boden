#ifndef BDN_Property_H_
#define BDN_Property_H_


#include <bdn/ReadProperty.h>

namespace bdn
{


/** Base class for property objects.

	Properties wrap an inner value like a String or an int (the value type
	is specified via the template parameter).

	The default / most common implementation of Property is DefaultProperty. 

	Usage
	=====

	When you want to expose a value in your class as a property then you should write
	a function that returns a reference to Property. Note that Property is an abstract base class,
	so the actual object you return must be an instance of one of its subclasses (usually DefaultProperty).
	But you should return it as a more general Property reference, so allow you to switch to another
	Property subclass later (see the section about setters and getters below).

	Read-only properties
	--------------------

	If the value is read-only then you should return ReadProperty instead (also an abstract base
	class). ReadProperty is a base class of Property.

	Example
	-------

	\code

	class MyClass
	{
	public:

		Property<String>& name()
		{
			return _name;
		}

	protected:
		DefaultProperty<String> _name;	// note that the object is actually a DefaultProperty instance
	};

	// Usage:

	MyClass myObject;
	myObject.name = "Hello";
	String value = myObject.name;	// value will be "Hello"

	\encode

	
	Functionality
	=============

	Properties serve three main functions:

	Data binding
	------------

	Properties can be bound to other properties (see bind()). When that other
	property changes then the bound property is automatically set to that value.


	Change notification
	-------------------

	Properties can notify observers when their value is changed. Observers can register for
	notification via onChange().


	Replacement for setters / getters
	---------------------------------

	Some Property subclasses allow a setter and/or getter function to be set. So if it becomes
	necessary to add special processing when the property value is read or written then that can
	be done by settings these functions "behind the scenes". The public interface and usage of
	the Property object are not affected by setting a getter and/or setter, so this can be postponed
	to a time when they are really necessary.

	At the point in time when you really need a  setter and/or getter, switch the actual implementation
	Property class to DelegatingProperty. Then you can set your setter and getter functions.
	

	Thread safety
	=============

	Properties are entirely thread safe. It is fine to manipulate them from multiple threads.

	
	
	Overhead
	========

	Properties usually have a certain overhead.	They are a little bit bigger than their wrapped value,
	usually needing about 10-30 bytes of additional space in their initial state (depending on the implementation
	class).

	When properties are bound or change listeners are added then these also add a little bit of
	additional overhead.

	Access to the property values is also a little slower, because they are thread safe and
	need to perform synchronization.

	However, you usually do not need to concern yourself with this. The additional overhead is
	small and in a typical program you will not notice any difference in overall memory usage
	or overall speed when using Properties.

	But, like any useful tool they can be overused. It is not recommended to use properties for
	all members of all your classes - then the overhead might become noticeable.
	Instead you should think about whether or not the Property functionality might be useful in that part
	of the program. If the answer is yes, use Properties. If not, don't use them.

	*/
template<class ValType>
class Property : public ReadProperty<ValType>
{
public:
        
    virtual void set(const ValType& val)=0;

	virtual Property<ValType>& operator=(const ValType& val)=0;
	virtual Property<ValType>& operator=(const ReadProperty<ValType>& val)=0;

    
	/** Binds the property to the value of another property. When the other property
		changes then this property will be automatically updated to the same value.
		This value update happens asynchronously via the main thread event queue, so the
		value is not updated immediately when the other property changes.*/
    virtual void bind(const ReadProperty<ValType>& sourceProperty)=0;        
    

	
};




}

#endif
