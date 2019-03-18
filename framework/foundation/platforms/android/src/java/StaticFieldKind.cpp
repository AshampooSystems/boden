
#include <bdn/java/StaticFieldKind.h>

#include <bdn/java/Env.h>

#include <bdn/jni.h>

namespace bdn::java
{
    jobject StaticFieldKind::getObjectField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticObjectField(cls, fieldId);
    }

    jboolean StaticFieldKind::getBooleanField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticBooleanField(cls, fieldId);
    }

    jint StaticFieldKind::getIntField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticIntField(cls, fieldId);
    }

    jbyte StaticFieldKind::getByteField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticByteField(cls, fieldId);
    }

    jshort StaticFieldKind::getShortField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticShortField(cls, fieldId);
    }

    jlong StaticFieldKind::getLongField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticLongField(cls, fieldId);
    }

    jchar StaticFieldKind::getCharField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticCharField(cls, fieldId);
    }

    jfloat StaticFieldKind::getFloatField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticFloatField(cls, fieldId);
    }

    jdouble StaticFieldKind::getDoubleField(jclass cls, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetStaticDoubleField(cls, fieldId);
    }

    void StaticFieldKind::setObjectField(jclass cls, jfieldID fieldId, jobject value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticObjectField(cls, fieldId, value);
    }

    void StaticFieldKind::setBooleanField(jclass cls, jfieldID fieldId, jboolean value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticBooleanField(cls, fieldId, value);
    }

    void StaticFieldKind::setIntField(jclass cls, jfieldID fieldId, jint value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticIntField(cls, fieldId, value);
    }

    void StaticFieldKind::setByteField(jclass cls, jfieldID fieldId, jbyte value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticByteField(cls, fieldId, value);
    }

    void StaticFieldKind::setShortField(jclass cls, jfieldID fieldId, jshort value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticShortField(cls, fieldId, value);
    }

    void StaticFieldKind::setLongField(jclass cls, jfieldID fieldId, jlong value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticLongField(cls, fieldId, value);
    }

    void StaticFieldKind::setCharField(jclass cls, jfieldID fieldId, jchar value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticCharField(cls, fieldId, value);
    }

    void StaticFieldKind::setFloatField(jclass cls, jfieldID fieldId, jfloat value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticFloatField(cls, fieldId, value);
    }

    void StaticFieldKind::setDoubleField(jclass cls, jfieldID fieldId, jdouble value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetStaticDoubleField(cls, fieldId, value);
    }
}
