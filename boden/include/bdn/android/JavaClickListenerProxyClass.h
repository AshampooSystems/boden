#ifndef _BDN_JavaClickListenerProxyClass_H_
#define _BDN_JavaClickListenerProxyClass_H_

#include <bdn/JavaConnector.h>

#include <jni.h>

namespace bdn
{

class JavaClickListenerProxyClass : public Base
{
public:
    JavaClickListenerProxyClass()
    {
        _pConn = JavaConnector::get();
        _class = _pConn->getClass("com/example/hazard/hello/ClickListenerProxy", false);
    }

    static JavaClickListenerProxyClass* get()
    {
        static JavaClickListenerProxyClass o;
        return &o;
    }

    jobject newInstance(jobject wrappedCppButton)
    {
        if(_constructorId==nullptr)
            _constructorId = _pConn->getMethodId(_class, "<init>", "(Ljava/lang/Object;)V");

        jobject obj = _pConn->getEnv()->NewObject( _class, _constructorId, wrappedCppButton );
        _pConn->throwExceptionFromLastJavaCall();

        return _pConn->newPersistentReference(obj);
    }


protected:
    JavaConnector*  _pConn;
    jclass          _class;
    jmethodID       _constructorId = nullptr;
};

}


#endif


