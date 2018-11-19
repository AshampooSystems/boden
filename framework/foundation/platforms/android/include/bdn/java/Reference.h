#ifndef BDN_JAVA_Reference_H_
#define BDN_JAVA_Reference_H_

#include <jni.h>

namespace bdn
{
    namespace java
    {

        /** Represents a strong reference to an object in the "Java world".
         *
         *  "Strong" means that the Reference object will keep the java-side
         * object alive. The java-side object will not be garbage collected
         * while the reference exists.
         *
         *  There is also a separate WeakReference class that enables one to
         * store a "non-strong" reference to a Java object that allows the
         * object to be garbage collected. See WeakReference for more
         * information on that.
         *
         *  Strong Reference objects are created from other Reference objects or
         * from jobject JNI value.
         *
         *  For converting jobject values, you have to use the static creation
         * functions convertExternalLocal(), convertAndDestroyOwnedLocal() or
         * wrapStrongGlobal().*
         *
         * */
        class Reference : public Base
        {
          public:
            /** Creates a Reference instance from an external local jobject.
             * External local jobjects are those that JNI callback functions get
             * as function parameters. They are under the control of the java
             * side and are not owned by the native code.*/
            static Reference convertExternalLocal(jobject localRef);

            /** Creates a Reference instance from an owned local jobject. Owned
             * local jobjects are those that JNI object creation functions
             * return.
             *
             *  convertAndDestroyOwnedLocal will destroy the jobject - it should
             * not be used afterwards.
             *  */
            static Reference convertAndDestroyOwnedLocal(jobject localRef);

            /** Creates a reference from a strong global jobject. The jobject is
             * simply wrapped - no new actual java-side reference is created.*/
            static Reference wrapStrongGlobal(jobject strongGlobalRef);

            /** Copy constructor.*/
            Reference(const Reference &o) : _shared(o._shared) {}

            /** Move constructor.*/
            Reference(Reference &&o) : _shared(std::move(o._shared)) {}

            /** Constructs a null-reference*/
            Reference() {}

            Reference &operator=(const Reference &o)
            {
                _shared = o._shared;

                return *this;
            }

            jobject getJObject() const
            {
                if (_shared == nullptr)
                    return NULL;
                else
                    return _shared->getJObject();
            }

            /** Returns true if the reference is null.
             *
             *  Note that calling this on a reference of the "weak" type does
             * not make any sense. Weak references can become null at ANY point
             * in time, even right after they have been checked for null. So
             * even if isNull returns true for a weak reference, that
             * information is immediately outdated and it is not safe to use the
             * reference. Weak references should first be converted to strong
             * references with toStrong(). Those do not have this problem and
             * then isNull can be safely called on the strong reference.
             * */
            bool isNull() const;

            /** Returns true if the reference is null.*/
            bool operator==(std::nullptr_t) const { return isNull(); }

            /** Returns true if this reference points to the same object as the
             * other reference.*/
            bool operator==(const Reference &o) const;

            /** Returns true if the reference is null.*/
            bool operator!=(std::nullptr_t) const { return !isNull(); }

            /** Returns true if this reference points to the same object as the
             * other reference.*/
            bool operator!=(const Reference &o) const { return !operator==(o); }

          private:
            Reference(jobject strongRef) : _shared(newObj<Shared>(strongRef)) {}

            class Shared : public Base
            {
              public:
                Shared(jobject ref) { _ref = ref; }

                ~Shared();

                jobject getJObject() const { return _ref; }

              protected:
                jobject _ref;
            };

            P<Shared> _shared;
        };
    }
}

#endif
