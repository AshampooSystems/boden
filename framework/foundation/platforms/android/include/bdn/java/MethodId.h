#ifndef BDN_JAVA_MethodId_H_
#define BDN_JAVA_MethodId_H_

namespace bdn
{
    namespace java
    {

        class MethodId;
        class JClass;
    }
}

#include <jni.h>

namespace bdn
{
    namespace java
    {

        /** Represents the ID of a Java method.
         *  These IDs are used to call Java methods from C++ code.
         *  See JObject::invoke() and JClass::invokeObjectMethod().*/
        class MethodId
        {
          public:
            MethodId() { _initialized = false; }

            /** Initializes the Id by looking up the method with the specified
             * name and signature from the specified class.
             *
             *  Throws a corresponding JavaException if the method was not
             * found.
             *
             *  It is usually not necessary to call this directly.
             *  JObject::invoke_() or JClass::invokeObjectMethod_() provide a
             * higher level access that automatically initializes the method Id.
             *  */
            void init(JClass &cls, const String &methodName,
                      const String &methodSignature);

            /** Initializes the Id by looking up the static method with the
             * specified name and signature from the specified class.
             *
             *  Throws a corresponding JavaException if the method was not
             * found.
             *
             *  It is usually not necessary to call this directly.
             *  JObject::invokeStatic_() or JClass::invokeStatictMethod_()
             * provide a higher level access that automatically initializes the
             * method Id.
             *  */
            void initStatic(JClass &cls, const String &methodName,
                            const String &methodSignature);

            /** Initializes the method Id object.*/
            void init(jmethodID id)
            {
                _id = id;
                _initialized = true;
            }

            bool isInitialized() const { return _initialized; }

            /** Returns the Id. Throws and exception if the Id has not been
             * initialized yet.*/
            jmethodID getId() const
            {
                if (!isInitialized())
                    throw ProgrammingError(
                        "MethodId::getId called on uninitialized object.");

                return _id;
            }

          private:
            jmethodID _id;
            bool _initialized;
        };
    }
}

#include <bdn/java/JObject.h>

#endif
