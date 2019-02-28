#pragma once

#include <functional>
#include <utility>

#include <bdn/DanglingFunctionError.h>

namespace bdn
{

    /** Internal class. Do not use directly.
     */
    template <class ObjectType, class MethodType> class StrongMethod_
    {
      public:
        StrongMethod_() : _method(nullptr) {}

        StrongMethod_(std::shared_ptr<ObjectType> object, MethodType method) : _object(object), _method(method) {}

        template <class... ArgTypes>
        typename std::result_of<MethodType(ObjectType *, ArgTypes...)>::type operator()(ArgTypes &&... args) const
        {
            if (!isValid())
                throw std::bad_function_call();

            return (*(_object.get()).*_method)(std::forward<ArgTypes>(args)...);
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
        std::shared_ptr<ObjectType> _object;
        MethodType _method;
    };

    /** Internal class. Do not use directly.
     */
    template <class ObjectType, class MethodType> class WeakMethod_
    {
      public:
        WeakMethod_() : _method(nullptr), _valid(false) {}

        WeakMethod_(std::weak_ptr<ObjectType> object, MethodType method)
            : _objectWeak(object), _method(method), _valid((object.lock() != nullptr) && method != nullptr)
        {}

        template <class... ArgTypes>
        typename std::result_of<MethodType(ObjectType *, ArgTypes...)>::type operator()(ArgTypes &&... args) const
        {
            if (!isValid())
                throw std::bad_function_call();

            std::shared_ptr<ObjectType> object = _objectWeak.lock();

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
        std::weak_ptr<ObjectType> _objectWeak;
        MethodType _method;
        bool _valid;
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

         std::shared_ptr<MyClass> object = std::make_shared<MyClass>();

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
    std::function<FuncType> strongMethod(const std::shared_ptr<ObjectType> &object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return StrongMethod_<ObjectType, FuncType(ObjectType::*)>(object, method);
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
    std::function<FuncType> weakMethod(std::weak_ptr<ObjectType> object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return WeakMethod_<ObjectType, FuncType(ObjectType::*)>(object, method);
    }

    template <class ObjectType, typename FuncType>
    std::function<FuncType> weakMethod(const std::shared_ptr<ObjectType> &object, FuncType ObjectType::*method)
    {
        if (object == nullptr || method == nullptr)
            return std::function<FuncType>();
        else
            return WeakMethod_<ObjectType, FuncType(ObjectType::*)>(object, method);
    }
}
