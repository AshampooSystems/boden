#pragma once

#include <bdn/ProgrammingError.h>
#include <bdn/String.h>

namespace bdn::java
{
    class MethodId;

    namespace wrapper
    {
        class Class;
    }
}

#include <bdn/jni.h>

namespace bdn::java
{
    /** Represents the ID of a Java method.
     *  These IDs are used to call Java methods from C++ code.
     *  See JObject::invoke() and JClass::invokeObjectMethod().*/
    class MethodId
    {
      public:
        constexpr MethodId() = default;

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
        void init(wrapper::Class &cls, const String &methodName, const String &methodSignature);

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
        void initStatic(wrapper::Class &cls, const String &methodName, const String &methodSignature);

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
            if (!isInitialized()) {
                throw ProgrammingError("MethodId::getId called on uninitialized object.");
            }

            return _id;
        }

      private:
        jmethodID _id{nullptr};
        bool _initialized{false};
    };
}
#include <bdn/java/wrapper/Object.h>
