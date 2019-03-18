
#include <bdn/java/ObjectFieldKind.h>

#include <bdn/java/Env.h>

#include <bdn/jni.h>

namespace bdn::java
{
    jobject ObjectFieldKind::getObjectField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetObjectField(obj, fieldId);
    }

    jboolean ObjectFieldKind::getBooleanField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetBooleanField(obj, fieldId);
    }

    jint ObjectFieldKind::getIntField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetIntField(obj, fieldId);
    }

    jbyte ObjectFieldKind::getByteField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetByteField(obj, fieldId);
    }

    jshort ObjectFieldKind::getShortField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetShortField(obj, fieldId);
    }

    jlong ObjectFieldKind::getLongField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetLongField(obj, fieldId);
    }

    jchar ObjectFieldKind::getCharField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetCharField(obj, fieldId);
    }

    jfloat ObjectFieldKind::getFloatField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetFloatField(obj, fieldId);
    }

    jdouble ObjectFieldKind::getDoubleField(jobject obj, jfieldID fieldId)
    {
        // note that this JNI call never throws a Java-side exception
        return Env::get().getJniEnv()->GetDoubleField(obj, fieldId);
    }

    void ObjectFieldKind::setObjectField(jobject obj, jfieldID fieldId, jobject value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetObjectField(obj, fieldId, value);
    }

    void ObjectFieldKind::setBooleanField(jobject obj, jfieldID fieldId, jboolean value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetBooleanField(obj, fieldId, value);
    }

    void ObjectFieldKind::setIntField(jobject obj, jfieldID fieldId, jint value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetIntField(obj, fieldId, value);
    }

    void ObjectFieldKind::setByteField(jobject obj, jfieldID fieldId, jbyte value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetByteField(obj, fieldId, value);
    }

    void ObjectFieldKind::setShortField(jobject obj, jfieldID fieldId, jshort value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetShortField(obj, fieldId, value);
    }

    void ObjectFieldKind::setLongField(jobject obj, jfieldID fieldId, jlong value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetLongField(obj, fieldId, value);
    }

    void ObjectFieldKind::setCharField(jobject obj, jfieldID fieldId, jchar value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetCharField(obj, fieldId, value);
    }

    void ObjectFieldKind::setFloatField(jobject obj, jfieldID fieldId, jfloat value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetFloatField(obj, fieldId, value);
    }

    void ObjectFieldKind::setDoubleField(jobject obj, jfieldID fieldId, jdouble value)
    {
        // note that this JNI call never throws a Java-side exception
        Env::get().getJniEnv()->SetDoubleField(obj, fieldId, value);
    }
}
