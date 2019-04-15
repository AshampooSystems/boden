path: tree/master/framework/foundation/include/bdn/property
source: Property.h

# Property

Represents a property of a class.

Properties provide a simple way to access and observe arbitrary value types such as `int` or `String`. You may observe value changes using the `onChange()` method or bind two or more properties using the `bind()` method.

See also: [Property Guide](../../guides/foundation/properties.md)

## Declaration

```C++
template <class ValType> 
class Property : virtual public IValueAccessor<ValType>
```

## Creating a Property Object

* **Property()**

	Constructs a property with a default-constructed value.

* **Property(const Property &) = delete**

	`Property` instances cannot be copy-constructed. However, they can be default-constructed and then assigned using `operator =`. See the [Property Guide](../../guides/foundation/properties.md#copying) for details.

* **Property(ValType value)**

	Initializes the property's value with the given value.

* **Property(const GetterSetter<ValType> &getterSetter)**

	Constructs a `Property` instance from a `GetterSetter` object. This can be used to define custom getter and setter methods. See the [Property Guide](../../guides/foundation/properties.md#getters-and-setters) for details.

* **Property([Streaming](streaming.md) &stream)**

	Constructs a `Property` instance with the given [`Streaming`](streaming.md) object.

* **template <class U\> Property(const [Transform](transform.md)<ValType, U\> &transform)**

	Constructs a `Property` instance with the given [`Transform`](transform.md) object.


## Getting and Modifying the Value

* **ValType get() const**

	Returns the current value.

* **void set(const ValType &value, bool notify = true)**

	Sets a new value. you can disable notifications via the notify argument.

* **ValType operator\*() const**

	Returns the property's value. Intended for use with the `auto` keyword.

* **operator ValType() const**

	Returns the property's value when casting explicitly or implicitly.

## Accessing Non-primitive Value Types

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

	Provides access to members of non-primitive pointer types.

## Binding Properties

* **void bind(Property<ValType\> &sourceProperty, BindMode bindMode = BindMode::bidirectional)**

	Binds the property to the given source property.

	`bindMode` can be one of `BindMode::unidirectional` (one-way) or `BindMode::bidirectional` (two-way). If `bindMode` is set to `BindMode::unidirectional`, the property's value will be updated when the source property's value is changed. However, the source property's value will not be updated when the property's value is changed. If `bindMode` is set to `BindMode::bidirectional`, either property will be updated when the other's value is changed. This is the default behavior of `bind()`.

	Property bindings work synchronously. That is, the bound property will be updated immediately on the thread the value change has been invoked on.

## Being Notified of Changes

* **auto &onChange() const**
	
	Returns a [Notifier](notifier.md) which is called when the property value changes.

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
