#ifndef BDN_JAVA_StaticFieldKind_H_
#define BDN_JAVA_StaticFieldKind_H_

#include <bdn/java/Env.h>
#include <bdn/java/JClass.h>

namespace bdn
{
    namespace java
    {

        class StaticFieldKind
        {
          public:
            typedef jclass ContextJType;

            /** Represents the ID of a Java static class field.
             *  These IDs are used to call access an object's java-side fields
             * from native code code. See #FieldAccessor.*/
            template <class NativeType> class Id
            {
              public:
                Id(JClass &cls, const char *fieldName)
                {
                    Env &env = Env::get();

                    _id = env.getJniEnv()->GetStaticFieldID(
                        (jclass)cls.getJObject_(), fieldName,
                        TypeConversion<NativeType>::getJavaSignature()
                            .asUtf8Ptr());

                    env.throwAndClearExceptionFromLastJavaCall();
                }

                /** Returns the Id. Throws and exception if the Id has not been
                 * initialized yet.*/
                jfieldID getId() const { return _id; }

              private:
                jfieldID _id;
            };

            static jobject getObjectField(jclass cls, jfieldID fieldId);
            static jboolean getBooleanField(jclass cls, jfieldID fieldId);
            static jint getIntField(jclass cls, jfieldID fieldId);
            static jshort getShortField(jclass cls, jfieldID fieldId);
            static jbyte getByteField(jclass cls, jfieldID fieldId);
            static jlong getLongField(jclass cls, jfieldID fieldId);
            static jchar getCharField(jclass cls, jfieldID fieldId);
            static jfloat getFloatField(jclass cls, jfieldID fieldId);
            static jdouble getDoubleField(jclass cls, jfieldID fieldId);

            static void setObjectField(jclass cls, jfieldID fieldId,
                                       jobject value);
            static void setBooleanField(jclass cls, jfieldID fieldId,
                                        jboolean value);
            static void setIntField(jclass cls, jfieldID fieldId, jint value);
            static void setShortField(jclass cls, jfieldID fieldId,
                                      jshort value);
            static void setLongField(jclass cls, jfieldID fieldId, jlong value);
            static void setByteField(jclass cls, jfieldID fieldId, jbyte value);
            static void setCharField(jclass cls, jfieldID fieldId, jchar value);
            static void setFloatField(jclass cls, jfieldID fieldId,
                                      jfloat value);
            static void setDoubleField(jclass cls, jfieldID fieldId,
                                       jdouble value);
        };
    }
}

#endif
