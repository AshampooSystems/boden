#ifndef BDN_method_H_
#define BDN_method_H_

#include <utility>
#include <functional>

#include <bdn/DanglingFunctionError.h>

namespace bdn
{

    /** Internal class. Do not use directly.
     */
    template <class ObjectType, class MethodType> class StrongMethod_
    {
      public:
        StrongMethod_() : _method(nullptr) {}

        StrongMethod_(ObjectType *object, MethodType method) : _object(object), _method(method) {}

        template <class... ArgTypes>
        typename std::result_of<MethodType(ObjectType *, ArgTypes...)>::type operator()(ArgTypes &&... args) const
        {
            if (!isValid())
                throw std::bad_function_call();

            return (*(_object.getPtr()).*_method)(std::forward<ArgTypes>(args)...);
        }

        /** Returns true if the method pointer is valid.
            Invalid methods will throw an exception when they are called.*/
        bool isValid() const { return (_object != nullptr && _method != nullptr); }

        /** Returns true if the method is invalid (see isValid()).*/
        bool operator==(std::nullptr_t) const { return !isValid(); }

        /** Returns true if the method is valid (see isValid()).*/
        bool operator!=(std::nullptr_t) const { return isValid(); }

        /** Returns true if the method is valid.*/
        explicit operator bool() const { return isValid(); }

      private:
        P<ObjectType> _object;
        MethodType _method;
    };

    /** Internal class. Do not use directly.
     */
    template <class ObjectType, class MethodType> class WeakMethod_
    {
      public:
        WeakMethod_() : _method(nullptr), _valid(false) {}

        WeakMethod_(ObjectType *object, MethodType method)
            : _objectWeak(object), _method(method), _valid((object != nullptr) && method != nullptr)
        {}

        template <class... ArgTypes>
        typename std::result_of<MethodType(ObjectType *, ArgTypes...)>::type operator()(ArgTypes &&... args) const
        {
            if (!isValid())
                throw std::bad_function_call();

            P<ObjectType> object = _objectWeak.toStrong();

            if (object == nullptr)
                throw DanglingFunctionError();

            return ((*object).*_method)(std::forward<ArgTypes>(args)...);
        }

        /** Returns true if the method pointer is valid.
         Invalid methods will throw an exception when they are called.*/
        bool isValid() const { return _valid; }

        /** Returns true if the method is invalid (see isValid()).*/
        bool operator==(std::nullptr_t) const { return !isValid(); }

        /** Returns true if the method is valid (see isValid()).*/
        bool operator!=(std::nullptr_t) const { return isValid(); }

        /** Returns true if the method is valid.*/
        explicit operator bool() const { return isValid(); }

      private:
        WeakP<ObjectType> _objectWeak;
        MethodType _method;
        bool _valid;
    };

    /** Internal class. Do not use directly.
     */
    template <class ObjectType, class MethodType> class PlainMethod_
    {
      public:
        PlainMethod_() : _object(nullptr), _method(nullptr) {}

        PlainMethod_(ObjectType *object, MethodType method) : _object(object), _method(method) {}

        template <class... ArgTypes>
        typename std::result_of<MethodType(ObjectType *, ArgTypes...)>::type operator()(ArgTypes &&... args) const
        {
            if (!isValid())
                throw std::bad_function_call();

            return ((*_object).*_method)(std::forward<ArgTypes>(args)...);
        }

        /** Returns true if the method pointer is valid.
         Invalid methods will throw an exception when they are called.*/
        bool isValid() const { return (_object != nullptr && _method != nullptr); }

        /** Returns true if the method is invalid (see isValid()).*/
        bool operator==(std::nullptr_t) const { return !isValid(); }

        /** Returns true if the method is valid (see isValid()).*/
        bool operator!=(std::nullptr_t) const { return isValid(); }

        /** Returns true if the method is valid.*/
        explicit operator bool() const { return isValid(); }

      private:
        ObjectType *_object;
        MethodType _method;
    };

    /**  Packages a method together with its object pointer and allows it to be
       called like a global function (without providing the object pointer at
       the point of the call).

         The wrapped method object holds a strong reference to the method's
       object. So the object is kept alive as long as the returned method object
       exists. See weakMethod() for an alternative that does not keep the object
       alive.

         You can also use this with virtual methods. They work as expected, just
       as if the method was called normally.

         Note that this function only works with methods of classes that
       implement IBase (which is automatically the case for all classes that
       derive from Base, i.e. for most classes in the Boden framework).

         Example:

         \code

         class MyClass : public Base
         {
         public:

            int hello(String s);
         };

         P<MyClass> object = newObj<MyClass>();

         std::function<int(String)> methodObj = strongMethod(object,
       &MyClass::hello);

         // the method can now be called without the need to provide object.
         int returnValue = methodObj("abc");

         // object can also be released - the method object will keep the
       object alive automatically. object = nullptr; returnValue =
       methodObj("abc");

         \endcode

         */
    template <class ObjectType, typename FuncType>
    std::function<FuncType> strongMethod(ObjectType *object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return StrongMethod_<ObjectType, FuncType(ObjectType::*)>(object, method);
    }

    template <class ObjectType, typename FuncType>
    std::function<FuncType> strongMethod(const P<ObjectType> &object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return StrongMethod_<ObjectType, FuncType(ObjectType::*)>(object.getPtr(), method);
    }

    /** This is similar to strongMethod(), except that the returned callable
       will not keep the method's object alive.

        IMPORTANT: weakMethod should only be used with objects that were
       allocated with newObj(). Using it with objects that were not allocated
       with newObj can lead to crashes and should be avoided.

        When the returned callable is called after the method's owning object
       was destroyed then a DanglingFunctionError exception will be thrown. Note
       that bdn::INotifier objects automatically handle this exception. They
       will remove the callable from their internal list and otherwise ignore
       the exception. So it is perfectly safe to use weak methods with
       Notifiers.

        */
    template <class ObjectType, typename FuncType>
    std::function<FuncType> weakMethod(ObjectType *object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return WeakMethod_<ObjectType, FuncType(ObjectType::*)>(object, method);
    }

    template <class ObjectType, typename FuncType>
    std::function<FuncType> weakMethod(const P<ObjectType> &object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return WeakMethod_<ObjectType, FuncType(ObjectType::*)>(object.getPtr(), method);
    }

    /** Packages a method together with its object pointer and allows it to be
       called like a global function (without providing the object pointer at
       the point of the call).

        The caller is responsible for ensuring that the method's object
       (object) is still valid at the time the function object is called. The
       returned function object does nothing to keep the method's object alive
       and it also does nothing to check the validity of the object. If the
       method's object is deleted then calling the returned function object will
        likely cause a crash, so use this carefully.

        plainMethod can be used with any object of any class. The object does
       not need to implement bdn::IBase.

        When to use
        -----------

        plainMethod is a bit more lightweight than weakMethod() and
       strongMethod() (although the performance benefit matters only very
       rarely).

        It is mainly intended for cases when you are certain that the object
       will be valid, or when the object does not implement bdn::IBase.

        */
    template <class ObjectType, typename FuncType>
    std::function<FuncType> plainMethod(ObjectType *object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return PlainMethod_<ObjectType, FuncType(ObjectType::*)>(object, method);
    }

    template <class ObjectType, typename FuncType>
    std::function<FuncType> plainMethod(const P<ObjectType> &object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return PlainMethod_<ObjectType, FuncType(ObjectType::*)>(object.getPtr(), method);
    }
}

#endif
