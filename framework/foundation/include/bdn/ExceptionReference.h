#ifndef BDN_ExceptionReference_H_
#define BDN_ExceptionReference_H_

namespace bdn
{

    /** Stores a reference to an arbitrary exception.

        This is very similar to std::exception_ptr. The main difference
        is that the class derived from Base and can be used with smart pointers
        (bdn::P).
        */
    class ExceptionReference : public Base
    {
      public:
        /** Constructs a null reference that does not point to an
            exception.*/
        ExceptionReference() noexcept {}

        ExceptionReference(const std::exception_ptr &p) noexcept
            : _exceptionPtr(p)
        {}

        ExceptionReference(const ExceptionReference &o) noexcept
            : _exceptionPtr(o._exceptionPtr)
        {}

        /** Returns an ExceptionReference object that refers to the currently
            active exception. An exception is active inside a catch clause.

            If no exception is active then a null reference is returned.

            If the creating the reference object involves allocating memory
           internally and there is not enough memory for that then the returned
           reference will refer to a std::bad_alloc exception.
            */
        static ExceptionReference fromActiveException() noexcept
        {
            return ExceptionReference(std::current_exception());
        }

        /** Similar to fromActiveException, except that the reference object
            is allocated with newObj and a pointer to the reference object is
           returned.
         */
        static P<ExceptionReference> newFromActiveException() noexcept
        {
            try {
                return newObj<ExceptionReference>(std::current_exception());
            }
            catch (std::bad_alloc &e) {
                return &getGlobalBadAllocReference();
            }
        }

        /** Creates an ExceptionReference object that points to a copy
            of the specified exception object.

            If the creating the reference object involves allocating memory
           internally and there is not enough memory for that then the returned
           reference will refer to a std::bad_alloc exception.*/
        template <class ParamType>
        static ExceptionReference fromException(ParamType exc) noexcept
        {
            return ExceptionReference(std::make_exception_ptr(exc));
        }

        /** Similar to fromException, except that the reference object
            is allocated with newObj and a pointer to the reference object is
           returned.
         */
        template <class ParamType>
        static P<ExceptionReference> newFromException(ParamType exc) noexcept
        {
            try {
                return newObj<ExceptionReference>(std::make_exception_ptr(exc));
            }
            catch (std::bad_alloc &e) {
                return &getGlobalBadAllocReference();
            }
        }

        /** Rethrows the exception.*/
        void rethrow() const { std::rethrow_exception(_exceptionPtr); }

        /** Returns true if the reference is null (i.e. if it does not point
            to an exception.*/
        bool isNull() const { return !_exceptionPtr; }

        /** Evaluates to true if the reference is not null.*/
        operator bool() const { return static_cast<bool>(_exceptionPtr); }

        /** Returns true if either both reference are null, or if they
            point to the same exception object.*/
        bool operator==(const ExceptionReference &o) const
        {
            return _exceptionPtr == o._exceptionPtr;
        }

        /** Inverse of operator==*/
        bool operator!=(const ExceptionReference &o) const
        {
            return _exceptionPtr != o._exceptionPtr;
        }

      private:
        static ExceptionReference &getGlobalBadAllocReference() noexcept
        {
            // note that we do not use safe static here, because safe static
            // involves allocating with new. And we need the global bad alloc
            // reference when there is not enough memory, so new should be
            // avoided.
            static ExceptionReference ref(
                std::make_exception_ptr(std::bad_alloc()));

            return ref;
        }

        std::exception_ptr _exceptionPtr;
    };
}

#endif
