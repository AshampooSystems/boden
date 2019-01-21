#pragma once

#include <bdn/java/Env.h>
#include <bdn/java/JClass.h>

namespace bdn
{
    namespace java
    {

        class ObjectFieldKind
        {
          public:
            typedef jobject ContextJType;

            /** Represents the ID of a Java object field.
             *  These IDs are used to call access an object's java-side fields
             * from native code code. See #FieldAccessor.*/
            template <class NativeType> class Id
            {
              public:
                Id(JClass &cls, const char *fieldName)
                {
                    Env &env = Env::get();

                    _id = env.getJniEnv()->GetFieldID((jclass)cls.getJObject_(), fieldName,
                                                      TypeConversion<NativeType>::getJavaSignature().c_str());

                    env.throwAndClearExceptionFromLastJavaCall();
                }

                /** Returns the Id. Throws and exception if the Id has not been
                 * initialized yet.*/
                jfieldID getId() const { return _id; }

              private:
                jfieldID _id;
            };

            static jobject getObjectField(jobject obj, jfieldID fieldId);
            static jboolean getBooleanField(jobject obj, jfieldID fieldId);
            static jint getIntField(jobject obj, jfieldID fieldId);
            static jshort getShortField(jobject obj, jfieldID fieldId);
            static jbyte getByteField(jobject obj, jfieldID fieldId);
            static jlong getLongField(jobject obj, jfieldID fieldId);
            static jchar getCharField(jobject obj, jfieldID fieldId);
            static jfloat getFloatField(jobject obj, jfieldID fieldId);
            static jdouble getDoubleField(jobject obj, jfieldID fieldId);

            static void setObjectField(jobject obj, jfieldID fieldId, jobject value);
            static void setBooleanField(jobject obj, jfieldID fieldId, jboolean value);
            static void setIntField(jobject obj, jfieldID fieldId, jint value);
            static void setShortField(jobject obj, jfieldID fieldId, jshort value);
            static void setLongField(jobject obj, jfieldID fieldId, jlong value);
            static void setByteField(jobject obj, jfieldID fieldId, jbyte value);
            static void setCharField(jobject obj, jfieldID fieldId, jchar value);
            static void setFloatField(jobject obj, jfieldID fieldId, jfloat value);
            static void setDoubleField(jobject obj, jfieldID fieldId, jdouble value);
        };
    }
}
