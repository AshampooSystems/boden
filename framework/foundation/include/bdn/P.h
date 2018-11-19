#ifndef BDN_P_H_
#define BDN_P_H_

namespace bdn
{

    /** A smart pointer class that automatically deletes objects when they are
       not needed anymore.

        This is implemented via reference counting with objects that implement
       #IBase (i.e. which most classes in this library implement).

        Note that in contrast to std::shared_ptr it is safe to mix and match P
       pointers with plain pointers. For example, the following is safe and
       results in the expected behaviour.

        \code
        P<SomeClass> a = newObj<SomeClass>();
        SomeClass* plainA = a;
        P<SomeClass b = plainA;
        \endcode

        The only thing you need to keep in mind when using #P pointers is to
       avoid circular references. If an object A holds a pointer to an object B
       and B also holds a pointer to A then both objects will keep each other
       alive indefinitely and will never be deleted. If you need such circular
       references then one of them should be either a plain (non-smart) pointer
        or a weak pointer (see #WeakP).
        */
    template <class T> class P
    {
      public:
        P() : _object(nullptr) {}

        P(std::nullptr_t) : _object(nullptr) {}

        P(const P &p) : P(p._object) {}

        P(P &&p) : _object(p.detachPtr()) {}

        template <class F> inline P(const P<F> &p) : P(p.getPtr()) {}

        template <class F> inline P(P<F> &&p) : _object(p.detachPtr()) {}

        P(T *p) : _object(p)
        {
            if (_object != nullptr)
                _object->addRef();
        }

        ~P()
        {
            if (_object != nullptr)
                _object->releaseRef();
        }

        /** Assigns a pointer to an object to the smart pointer and increases
            the object's reference count.

            If the smart pointer already held a reference to another object then
            that reference is released.*/
        void assign(T *obj)
        {
            T *myOld = _object;

            // it is important that we increase the reference count on the new
            // object first. Otherwise the following could cause the object to
            // be deleted by mistake: P<SomeClass> p = ...; p = p;

            _object = obj;
            if (_object != nullptr)
                _object->addRef();

            if (myOld != nullptr)
                myOld->releaseRef();
        }

        void assign(P &&obj)
        {
            attachPtr(obj.getPtr());
            obj.detachPtr();
        }

        template <class F> void assign(P<F> &&obj)
        {
            attachPtr(obj.getPtr());
            obj.detachPtr();
        }

        P<T> &operator=(T *obj)
        {
            assign(obj);
            return *this;
        }

        P<T> &operator=(const P<T> &obj)
        {
            assign(obj.getPtr());
            return *this;
        }

        P<T> &operator=(P<T> &&obj)
        {
            attachPtr(obj.getPtr());
            obj.detachPtr();

            return *this;
        }

        template <class O> inline P<T> &operator=(const P<O> &obj)
        {
            assign(obj.getPtr());
            return *this;
        }

        template <class O> inline P<T> &operator=(P<O> &&obj)
        {
            attachPtr(obj.getPtr());
            obj.detachPtr();

            return *this;
        }

        /** Detaches the object without releasing the reference (without
           decrementing its reference counter).

            Afterwards the smart pointer will be null.

            Returns a plain pointer to the object that the smart pointer pointed
           to.
            */
        T *detachPtr()
        {
            T *obj = _object;

            _object = nullptr;

            return obj;
        }

        /** Sets the smart pointer to point to an object without incrementing
           its reference count.

            The object's reference count will be released as normal when the
           smart pointer is deleted or gets another pointer assigned.
        */
        P &attachPtr(T *obj)
        {
            if (_object != nullptr)
                _object->releaseRef();

            _object = obj;

            return *this;
        }

        /** Conversion operator to a plain pointer.*/
        operator T *() const { return _object; }

        /** Allows direct access to the members that this pointer points to.*/
        T *operator->() const { return _object; }

        bool operator==(const P<T> &p) const { return _object == p._object; }

        bool operator==(T *p) const { return _object == p; }

        bool operator==(std::nullptr_t) const { return _object == nullptr; }

        bool operator!=(const P<T> &obj) const { return _object != obj._object; }

        bool operator!=(T *obj) const { return _object != obj; }

        bool operator!=(std::nullptr_t) const { return _object != nullptr; }

        /** Returns the value of the smart pointer as a plain pointer.*/
        T *getPtr() const { return _object; }

      protected:
        T *_object;
    };
}

#endif
