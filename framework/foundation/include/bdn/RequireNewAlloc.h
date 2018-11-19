#ifndef BDN_RequireNewAlloc_H_
#define BDN_RequireNewAlloc_H_

#include <bdn/Thread.h>

namespace bdn
{

    /** Returns a reference to a thread-local static variable. This is used by
        RequireNewAlloc to track wether or not an object was allocated with new.
        */
    bool &_requireNewAlloc_getThreadLocalAllocatedWithNewRef();

    /** Wraps a class and adds special code that enforces that instances of the
       class are only created with new or newObj.

        If an instance is created as a local or member
        variable then the constructor will throw an exception.

        The ActualType template parameter is used to generate a better error
       message if the object is not allocated correctly. It should be the type
       of the class that derives from RequireNewAlloc.

        Example:

        \code

        class MyClass : public RequireNewAlloc<Base, MyClass>
        {
        public:
            ...
        };

        // this will cause an exception to be thrown:
        MyClass obj;

        // this will work
        P<MyClass> obj = newObj< MyClass >();



        */
    template <class BaseType, class ActualType> class RequireNewAlloc : public BaseType
    {
      public:
        template <typename... Args> RequireNewAlloc(Args &&... args) : BaseType(std::forward<Args>(args)...)
        {
            bool &allocatedWithNewRef = _requireNewAlloc_getThreadLocalAllocatedWithNewRef();

            // get the current value
            bool allocatedWithNew = allocatedWithNewRef;

            // reset the value
            allocatedWithNewRef = false;

            if (!allocatedWithNew) {
                // note that we cannot use typeid(*this).name() to get the type
                // of the object. Since construction of the class deriving from
                // us has not finished yet, it is likely that typeid(*this) will
                // simply return the type of RequireNewAlloc<BaseType>. Because
                // of this we added the additional template parameter
                // ActualType, which we use to get a better name.

                programmingError("Instances of " + String(typeid(ActualType).name()) +
                                 " must be allocated with newObj or new.");
            }
        }

        static inline void *operator new(size_t size, Base::RawNew r)
        {
            _requireNewAlloc_getThreadLocalAllocatedWithNewRef() = true;

            return BaseType::operator new(size, r);
        }
    };
}

#endif
