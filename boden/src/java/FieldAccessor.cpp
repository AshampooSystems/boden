#include <bdn/init.h>
#include <bdn/java/FieldAccessor.h>

#include <bdn/java/Env.h>

#include <jni.h>

namespace bdn
{
namespace java
{

jobject  FieldAccessorBase_::getObjectField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetObjectField(obj, fieldId);
}

jboolean FieldAccessorBase_::getBooleanField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetBooleanField(obj, fieldId);
}

jint     FieldAccessorBase_::getIntField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetIntField(obj, fieldId);
}

jbyte     FieldAccessorBase_::getByteField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetByteField(obj, fieldId);
}

jshort   FieldAccessorBase_::getShortField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetShortField(obj, fieldId);
}

jlong    FieldAccessorBase_::getLongField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetLongField(obj, fieldId);
}

jchar    FieldAccessorBase_::getCharField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetCharField(obj, fieldId);
}

jfloat   FieldAccessorBase_::getFloatField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetFloatField(obj, fieldId);
}

jdouble  FieldAccessorBase_::getDoubleField(jobject obj, jfieldID fieldId)
{
    return Env::get().getJniEnv()->GetDoubleField(obj, fieldId);
}


void FieldAccessorBase_::setObjectField(jobject obj, jfieldID fieldId, jobject value)
{
    Env::get().getJniEnv()->SetObjectField(obj, fieldId, value);
}

void FieldAccessorBase_::setBooleanField(jobject obj, jfieldID fieldId, jboolean value)
{
    Env::get().getJniEnv()->SetBooleanField(obj, fieldId, value);
}

void FieldAccessorBase_::setIntField(jobject obj, jfieldID fieldId, jint value)
{
    Env::get().getJniEnv()->SetIntField(obj, fieldId, value);
}

void FieldAccessorBase_::setByteField(jobject obj, jfieldID fieldId, jbyte value)
{
    Env::get().getJniEnv()->SetByteField(obj, fieldId, value);
}

void FieldAccessorBase_::setShortField(jobject obj, jfieldID fieldId, jshort value)
{
    Env::get().getJniEnv()->SetShortField(obj, fieldId, value);
}

void FieldAccessorBase_::setLongField(jobject obj, jfieldID fieldId, jlong value)
{
    Env::get().getJniEnv()->SetLongField(obj, fieldId, value);
}

void FieldAccessorBase_::setCharField(jobject obj, jfieldID fieldId, jchar value)
{
    Env::get().getJniEnv()->SetCharField(obj, fieldId, value);
}

void FieldAccessorBase_::setFloatField(jobject obj, jfieldID fieldId, jfloat value)
{
    Env::get().getJniEnv()->SetFloatField(obj, fieldId, value);
}

void FieldAccessorBase_::setDoubleField(jobject obj, jfieldID fieldId, jdouble value)
{
    Env::get().getJniEnv()->SetDoubleField(obj, fieldId, value);
}


}
}


