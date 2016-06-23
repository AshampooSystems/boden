#ifndef _BDN_JAVASWITCHCLASS_H_
#define _BDN_JAVASWITCHCLASS_H_

#include <bdn/JavaConnector.h>

#include <jni.h>

namespace bdn
{

class JavaSwitchClass : public Base
{
public:
    JavaSwitchClass()
    {
        _pConnector = JavaConnector::get();
        _class = _pConnector->getClass("android/widget/Switch", false);
    }

    static JavaSwitchClass* get()
    {
        static JavaSwitchClass o;
        return &o;
    }

    jobject newInstance(jobject contextObj)
    {
        if(_constructorId==nullptr)
            _constructorId = _pConnector->getMethodId(_class, "<init>", "(Landroid/content/Context;)V");

        jobject obj = _pConnector->getEnv()->NewObject(_class, _constructorId, contextObj );
        _pConnector->throwExceptionFromLastJavaCall();

        return _pConnector->newPersistentReference(obj);
    }

    void setText(jobject obj, const std::string& text)
    {
        if(_setTextId==nullptr)
            _setTextId = _pConnector->getMethodId(_class, "setText", "(Ljava/lang/CharSequence;)V");

        jstring textObj = _pConnector->newString(text);

        _pConnector->getEnv()->CallVoidMethod( obj, _setTextId, textObj);
        _pConnector->throwExceptionFromLastJavaCall();
    }


protected:
    JavaConnector*  _pConnector;
    jclass          _class;
    jmethodID       _constructorId = nullptr;
    jmethodID       _setTextId = nullptr;
};

}


#endif


