path: tree/master/framework/foundation/include/bdn/property
source: Property.h

# Property

Represents a property of a class.

Properties provide a simple means for accessing, manipulating and observing arbitrary value types such as int or String via the ValType
template parameter. You may observe value changes using the onChange() method or bind two or more properties using the bind() method.

See also: [Property Guide](../../../guides/foundation/properties.md)

## Declaration

```C++
template <class ValType> 
class Property : virtual public IValueAccessor<ValType>
```

## Constructor

* **Property()**

	Default constructor, value is uninitialized

* **Property(const Property &) = delete**

	The copy constructor is deleted, Property cannot be copied.

* **Property(ValType value)**

	Initializes the value with the given value

* **Property(const GetterSetter<ValType> &getterSetter)**

	Constructs a Property instance from a GetterSetter object 

* **Property(const Setter<ValType> &setter)**

	Constructs a Property instance with the given Setter object 

* **Property(Streaming &stream)**

	Constructs a Property instance with the given Streaming object

* **template <class U> Property(const Transform<ValType, U> &transform)**

	Constructs a Property instance with the given Transform object


## Value

* **ValType get() const**

	Returns the current value.

* **void set(const ValType &value, bool notify = true)**

	Sets a new value. you can disable notifications via the notify argument.

* **ValType operator\*() const**

	Returns the property's value – intended for use with the `auto` keyword

* **operator ValType() const**

	Returns the property's value when casting explicitly or implicitly

## Non-primitive value types

* **template<...> const ValType operator-\>() const**
* **template<...> const typename backing_t::Proxy operator-\>() const**

	```c++
	template <typename U=ValType, 
			  typename std::enable_if<overloadsArrowOperator<U>::value, int>::type = 0>
		const ValType operator->();
    
    template <typename U = ValType, 
    		  typename std::enable_if<!overloadsArrowOperator<U>::value, int>::type = 0>
    	const typename backing_t::Proxy operator->() const;
	```

	Provides access to members of non-primitive pointer types

## Binding

* **void bind(Property<ValType\> &sourceProperty, BindMode bindMode = BindMode::bidirectional)**

	Binds to the given property. Internally it adds a subscription on the `sourceProperty's onChange
	notification in which it calls set() on itself.

	When `bindMode` is `BindMode::bidirectional` it also add the inverse notification to the `sourceProperty`

	If [Compare<ValType\>::is_faked](compare.md) is true a std::logic_error is thrown if the user tries to create a bidirectional binding.

## Notification

* **auto &onChange() const**
	
	Returns a [Notifier](../notifier.md) thats called when the property value changes.

## Operators

* **Property &operator=(const ValType &value)**
* **bool operator==(const ValType &value) const**
* **bool operator==(const char \*cString) const**
* **bool operator!=(const ValType &value) const**
* **bool operator!=(const char \*cString) const**
* **Property &operator=(const Property &otherProperty)**
* **Property operator+() const**
* **Property operator-() const**
* **Property operator+(const Property &otherProperty) const**
* **Property operator-(const Property &otherProperty) const**
* **Property operator\*(const Property &otherProperty) const**
* **Property operator/(const Property &otherProperty) const**
* **Property operator%(const Property &otherProperty) const**
* **Property operator~() const**
* **Property operator&(const Property &otherProperty) const**
* **Property operator|(const Property &otherProperty) const**
* **Property operator^(const Property &otherProperty) const**
* **Property operator<<(const Property &otherProperty) const**
* **Property operator>>(const Property &otherProperty) const**
* **ValType operator+(const ValType &other) const**
* **ValType operator-(const ValType &other) const**
* **ValType operator\*(const ValType &other) const**
* **ValType operator/(const ValType &other) const**
* **ValType operator%(const ValType &other) const**
* **ValType operator&(const ValType &other) const**
* **ValType operator|(const ValType &other) const**
* **ValType operator^(const ValType &other) const**
* **ValType operator<<(const ValType &other) const**
* **ValType operator>>(const ValType &other) const**
* **Property &operator+=(const Property &otherProperty)**
* **Property &operator-=(const Property &otherProperty)**
* **Property &operator\*=(const Property &otherProperty)**
* **Property &operator/=(const Property &otherProperty)**
* **Property &operator%=(const Property &otherProperty)**
* **Property &operator&=(const Property &otherProperty)**
* **Property &operator|=(const Property &otherProperty)**
* **Property &operator^=(const Property &otherProperty)**
* **Property &operator<<=(const Property &otherProperty)**
* **Property &operator>>=(const Property &otherProperty)**
