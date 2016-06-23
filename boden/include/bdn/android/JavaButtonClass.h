#ifndef _BDN_JAVABUTTONCLASS_H_
#define _BDN_JAVABUTTONCLASS_H_

#include <bdn/JavaConnector.h>

#include <jni.h>

namespace bdn
{

class JavaButtonClass : public Base
{
public:
    JavaButtonClass()
    {
        _pConnector = JavaConnector::get();
        _class = _pConnector->getClass("android/widget/Button", false);
    }

    static JavaButtonClass* get()
    {
        static JavaButtonClass o;
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

    void setVisibility(jobject obj, int visibility)
    {
        if(_setVisibilityId==nullptr)
            _setVisibilityId = _pConnector->getMethodId(_class, "setVisibility", "(I)V");

        _pConnector->getEnv()->CallVoidMethod( obj, _setVisibilityId, visibility);
        _pConnector->throwExceptionFromLastJavaCall();
    }

    void setOnClickListener(jobject obj, jobject listenerObj)
    {
        if(_setOnClickListenerId==nullptr)
            _setOnClickListenerId = _pConnector->getMethodId(_class, "setOnClickListener", "(Landroid/view/View$OnClickListener;)V");

        _pConnector->getEnv()->CallVoidMethod( obj, _setOnClickListenerId, listenerObj);
        _pConnector->throwExceptionFromLastJavaCall();
    }


protected:
    JavaConnector*  _pConnector;
    jclass          _class;
    jmethodID       _constructorId = nullptr;
    jmethodID       _setTextId = nullptr;
    jmethodID       _setVisibilityId = nullptr;
    jmethodID       _setOnClickListenerId = nullptr;
};

}


#endif


