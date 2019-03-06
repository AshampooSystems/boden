#pragma once

#include <bdn/java/Env.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::java::wrapper
{
    /** Helper to handle Java Object arrays ( Object[] ).*/
    template <class ELEMENT_TYPE> class ArrayOfObjects : public Object
    {
      private:
        Reference _makeArray(size_t length)
        {
            bdn::java::Env &env = bdn::java::Env::get();
            JNIEnv *jniEnv = env.getJniEnv();

            Class &elementClass = ELEMENT_TYPE::getStaticClass_();

            jobjectArray result = jniEnv->NewObjectArray(length, (jclass)elementClass.getJObject_(), (jobject)0);

            env.throwAndClearExceptionFromLastJavaCall();

            return Reference::convertAndDestroyOwnedLocal(result);
        }

      public:
        /** @param objectRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit ArrayOfObjects(const Reference &objectRef) : Object(objectRef) {}

        explicit ArrayOfObjects(size_t length) : Object(_makeArray(length)) {}

        size_t getLength()
        {
            bdn::java::Env &env = bdn::java::Env::get();
            JNIEnv *jniEnv = env.getJniEnv();

            jarray javaRef = (jarray)getJObject_();
            jsize result = jniEnv->GetArrayLength(javaRef);

            env.throwAndClearExceptionFromLastJavaCall();

            return (size_t)result;
        }

        /** Returns the element at the specified index.*/
        ELEMENT_TYPE getElement(size_t index)
        {
            bdn::java::Env &env = bdn::java::Env::get();
            JNIEnv *jniEnv = env.getJniEnv();

            jobjectArray javaRef = (jobjectArray)getJObject_();
            jobject result = jniEnv->GetObjectArrayElement(javaRef, index);

            env.throwAndClearExceptionFromLastJavaCall();

            return ELEMENT_TYPE(Reference::convertAndDestroyOwnedLocal(result));
        }

        /** Sets the element at the specified index.*/
        void setElement(size_t index, ELEMENT_TYPE element)
        {
            bdn::java::Env &env = bdn::java::Env::get();
            JNIEnv *jniEnv = env.getJniEnv();

            jobjectArray javaRef = (jobjectArray)getJObject_();
            jniEnv->SetObjectArrayElement(javaRef, index, element.getJObject_());

            env.throwAndClearExceptionFromLastJavaCall();
        }

        /** Returns the JClass object for this class.
         *
         *  Note that the returned class object is not necessarily unique
         * for the whole process. You might get different objects if this
         * function is called from different shared libraries.
         *
         *  If you want to check for type equality then you should compare
         * the type name (see getTypeName() )
         *  */
        static Class &getStaticClass_()
        {
            static Class cls(ELEMENT_TYPE::getStaticClass_().getNameInSlashNotation_() + "[]");

            return cls;
        }

        Class &getClass_() override { return getStaticClass_(); }
    };
}