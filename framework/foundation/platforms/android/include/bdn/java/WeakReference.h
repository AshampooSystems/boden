#pragma once

#include <bdn/java/Reference.h>

namespace bdn::java
{
    /** Represents a weak reference to an object in the "Java world".
     *
     *  Weak references do not prevent the java-side object from being
     garbage collected.
     *  To access the object, you have to call toStrong() to convert the
     weak reference to a strong reference.

     *  The returned strong reference will then either be null (indicating
     that the java-side object does not
     *  exist anymore) or non-null. If it is non-null then you can access
     the object via the strong reference.
     *
     *  You can delete the strong reference again after the access, if you
     do not need it.
     * */
    class WeakReference : public Base
    {
      public:
        WeakReference(const Reference &o);

        /** Constructs a null-reference*/
        WeakReference() = default;

        /** Returns a strong reference to the Java object. The returned
         * reference may be null if the java-side object was already
         * destroyed.*/
        Reference toStrong() const;

        /** Temporarily converts the weak reference to a strong reference
         * and checks if the strong reference refers to the same object as
         * the specified one.*/
        bool operator==(const Reference &strongRef) const { return toStrong() == strongRef; }

        bool operator!=(const Reference &strongRef) const { return !operator==(strongRef); }

      private:
        WeakReference(jobject weakRef) : _shared(std::make_shared<Shared>(weakRef)) {}

        class Shared : public Base
        {
          public:
            Shared(jobject ref) { _ref = ref; }

            ~Shared() override;

            jobject getJObject() const { return _ref; }

          protected:
            jobject _ref;
        };

        std::shared_ptr<Shared> _shared;
    };
}
