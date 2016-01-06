#ifndef _BDN_JAVA_CONNECTOR_H_
#define _BDN_JAVA_CONNECTOR_H_

#include <bdn/Base.h>
#include <bdn/JniEnvNotSetError.h>
#include <bdn/JavaClassNotFoundError.h>
#include <bdn/JavaMethodNotFoundError.h>
#include <bdn/WrappedJavaError.h>

#include <jni.h>
#include <android/log.h>

#include <map>

namespace bdn
{

class JavaConnector : public Base
{
public:
    JavaConnector()
    {
    }

    static JavaConnector* get()
    {
        static JavaConnector o;

        return &o;
    }

    void setEnv(JNIEnv* pEnv)
    {
        _pEnv = pEnv;
    }

    JNIEnv* getEnv()
    {
        ensureHaveEnv();
        return _pEnv;
    }

    void setMainActivityObject(jobject obj)
    {
        _mainActivityObject = obj;
    }

    jobject getMainActivityObject()
    {
        return _mainActivityObject;
    }

    void setMainViewGroupObject(jobject obj)
    {
        _mainViewGroupObject = obj;
    }

    jobject getMainViewGroupObject()
    {
        return _mainViewGroupObject;
    }

    void logError(const std::string& message)
    {
        __android_log_write(ANDROID_LOG_ERROR, "boden", message.c_str());
    }

    void logInfo(const std::string& message)
    {
        __android_log_write(ANDROID_LOG_INFO, "boden", message.c_str());
    }

    jstring newString(const std::string& s)
    {
        ensureHaveEnv();
        jstring obj = _pEnv->NewStringUTF(s.c_str() );
        throwExceptionFromLastJavaCall();

        return obj;
    }


    jobject newPersistentReference(jobject obj)
    {
       ensureHaveEnv();

       return _pEnv->NewGlobalRef(obj);
    }

    void releasePersistentReference(jobject obj)
    {
       ensureHaveEnv();

       _pEnv->DeleteGlobalRef(obj);
    }


    /** Returns the Java class with the specified name.

        If autoRelease is true then the class reference is automatically
        released at the end of this JNI call.

        If autoRelease is false then you have to explicitly release
        the reference with #deleteReference
        */
    jclass getClass(const std::string& name, bool autoRelease)
    {
        ensureHaveEnv();

        jclass clazz = _pEnv->FindClass(name.c_str());
        if(clazz==NULL)
            throw JavaClassNotFoundError(name);
        throwExceptionFromLastJavaCall();

        if(!autoRelease)
            clazz = (jclass)_pEnv->NewGlobalRef(clazz);

        return clazz;
    }

    jmethodID getMethodId(jclass clazz, const std::string& name, const std::string& signature)
    {
        ensureHaveEnv();

        jmethodID methodId = _pEnv->GetMethodID(clazz, name.c_str(), signature.c_str());
        if(methodId==NULL)
            throw JavaMethodNotFoundError(name, signature);
        throwExceptionFromLastJavaCall();

        return methodId;
    }


    /** Creates a java.nio.ByteBuffer object that refers to the
        specified memory address.

        This can be used in two ways:

        1) to have Java directly manipulate the referenced memory.
        2) to simply wrap a C++ pointer as a Java object, with the intention
           of passing it through Java and receiving it back in C++ at some
           point in the future.

        If the memory is not interpreted by Java code then you can pass
        a dummy value (e.g. 1) for bufferCapacity.
        */
    jobject wrapPointer(void* pAddress, int bufferCapacity=1)
    {
        ensureHaveEnv();

        jobject obj = _pEnv->NewDirectByteBuffer(pAddress, bufferCapacity);
        throwExceptionFromLastJavaCall();

        return obj;
    }

    /** Unwraps a memory area that was previously wrapped in a Java
        ByteBuffer object with #wrapMemory. Returns a pointer to
        the memory area.*/
    void* unwrapPointer(jobject obj)
    {
        ensureHaveEnv();

        void* pAddress = _pEnv->GetDirectBufferAddress(obj);
        throwExceptionFromLastJavaCall();

        return pAddress;
    }



    /** If the last JAVA call produced an exception then this is thrown as a C++ exception.*/
    void throwExceptionFromLastJavaCall()
    {
        ensureHaveEnv();

        jthrowable exc = _pEnv->ExceptionOccurred();
        if(exc!=nullptr)
        {
            logError("Java method called by android native component threw an exception.");
            _pEnv->ExceptionDescribe();

            throw WrappedJavaError(exc);
        }
    }

    void convertToJavaException(std::exception& e)
    {
        ensureHaveEnv();

        WrappedJavaError* pWrappedErr = dynamic_cast<WrappedJavaError*>(&e);
        if(pWrappedErr!=nullptr)
        {
            logError("Passing Java exception through to Java environment.");
            _pEnv->Throw(pWrappedErr->getExceptionObject());
        }
        else
        {
            jclass exceptionClass = getClass("java/lang/Exception", true);

            std::string message = "Error from Android native component: "+std::string(e.what());

            logError("C++ exception occurred in android native component. Creating corresponding exception in Java environment: "+message);

            _pEnv->ThrowNew(exceptionClass, message.c_str() );
        }
    }



protected:
    void ensureHaveEnv()
    {
        if(_pEnv==nullptr)
            throw JniEnvNotSetError();
    }

    JNIEnv* _pEnv = nullptr;

    jobject _mainActivityObject = nullptr;
    jobject _mainViewGroupObject = nullptr;
};

}


#endif


