# Wrapping a java class

Wrapping a java class in Boden is done using the classes in ```bdn::java::wrapper```.
There are two classes, [Object](object.md) and [JTObject](jtobject.md). [JTObject](jtobject.md) is a convenience class that uses templates to make creating new wrapper simpler. 

## First steps

In this example we show the simplest way to wrap a java class:

```C++
constexpr const char kMyJavaClassName[] = "io/mycompany/MyJavaClass";
using MyJavaClass = bdn::java::wrapper::JTObject<kMyJavaClassName>;
```

With these definitions you can now instantiate a Java class from C++ in your code:

```C++
MyJavaClass myClass;
```

This will automatically instantiate the Java class for you. Of course this does not give you much yet, as no methods can be called.

## Wrapping java class methods

To be able to add methods to your Wrapper class we need to expand the previous example:

```C++
constexpr const char kMyJavaClassName[] = "io/mycompany/MyJavaClass";
class MyJavaClass : public bdn::java::wrapper::JTObject<kMyJavaClassName>
{
public:
	using bdn::java::wrapper::JTObject<kMyJavaClassName>::JTObject;
};
```

Now that we have our class we are ready to add methods to it. 
For this we are going to use the convenience class [JavaMethod](method.md).

```C++ hl_lines="7"
constexpr const char kMyJavaClassName[] = "io/mycompany/MyJavaClass";
class MyJavaClass : public bdn::java::wrapper::JTObject<kMyJavaClassName>
{
public:
	using bdn::java::wrapper::JTObject<kMyJavaClassName>::JTObject;
public:
	bdn::JavaMethod<void(bdn::JavaString)> testFunction{this, "testFunction"};
};
```

JavaMethod itself is a template that takes a function pointer style argument. It needs to be initialized with a pointer to the object (```this```) and the name of the function has be be provided as a string ( in this case "testFunction" )

Now we can call the function:

```C++
MyJavaClass myClass;
myClass.testFunction("Hello World");
```

## Deriving from JTObject based classes

To derive from a class, we need to pass through the new class name. Building on our previous example we change MyJavaClass to the following. ( Changes are highlighted )

```C++ hl_lines="2 3 6 10"
constexpr const char kMyJavaClassName[] = "io/mycompany/MyJavaClass";
template<const char* javaClassName = kMyJavaClassName>
class BaseMyJavaClass : public bdn::java::wrapper::JTObject<javaClassName>
{
public:
	using bdn::java::wrapper::JTObject<javaClassName>::JTObject;
public:
	bdn::JavaMethod<void(bdn::JavaString)> testFunction{this, "testFunction"};
};
using MyJavaClass = BaseMyJavaClass<>;
```

In the first highlighted line we change MyJavaClass to be a template class taking a ```const char*```
so we can override the java class name. As a convenience we set the original name as a default argument.

In the second line we've changed the class name to ```BaseMyJavaClass``` this again is for convenience. In the end we create a ```using``` declaration so our code can continue to use ```MyJavaClass```.

Now just as before we can create a new class that derives from BaseMyJavaClass:

```C++
constexpr const char kDerivedJavaClassName[] = "io/mycompany/DervivedJavaClass";
class DerivedJavaClass : public BaseMyJavaClass<kDerivedJavaClassName>
{
public:
	using BaseMyJavaClass<kDerivedJavaClassName>::BaseMyJavaClass;
};
```

With this we can instantiate the dervied class. You do not need to redefine methods if your java class overloads them. They will automatically work.

```C++
DerivedJavaClass object;
object.testFunction("Hello World");
```
