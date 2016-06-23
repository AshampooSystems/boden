#ifndef _BDN_JavaViewGroupClass_H_
#define _BDN_JavaViewGroupClass_H_

#include <bdn/JavaConnector.h>

#include <jni.h>

namespace bdn
{

class JavaViewGroupClass : public Base
{
public:
    JavaViewGroupClass()
    {
        _pConn = JavaConnector::get();
        _class = _pConn->getClass("android/view/ViewGroup", false);
    }


    static JavaViewGroupClass* get()
    {
        static JavaViewGroupClass o;
        return &o;
    }

    void addView(jobject obj, jobject subViewObj)
    {
        if(_addViewId==nullptr)
            _addViewId = _pConn->getMethodId(_class, "addView", "(Landroid/view/View;)V");

        _pConn->getEnv()->CallVoidMethod( obj, _addViewId, subViewObj);
        _pConn->throwExceptionFromLastJavaCall();
    }

protected:
    JavaConnector*  _pConn;
    jclass          _class;

    jmethodID       _addViewId = nullptr;
};

}


#endif


