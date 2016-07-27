#ifndef BDN_JAVA_Env_H_
#define BDN_JAVA_Env_H_

#include <bdn/java/JniEnvNotSetError.h>
#include <bdn/java/JavaException.h>
#include <bdn/java/JException.h>

#include <jni.h>


#define BDN_JNI_BEGIN(pEnv) \
    { \
        bdn::java::Env::JniBlock jniBlock_(pEnv); \
        try \
        {


#define BDN_JNI_END \
        } \
        catch(std::exception& e) \
        { \
            jniBlock_.endedWithException(e); \
            return; \
        } \
        catch(...) \
        { \
            jniBlock_.endedWithException(std::exception()); \
            return; \
        } \
        jniBlock_.endedNormally(); \
    }



namespace bdn
{
namespace java
{

class Env : public Base
{
public:
    Env()
    {
    }

    BDN_SAFE_STATIC_THREAD_LOCAL(Env, get );


    /** Helper class that manages the environment inside a JNI callback function.
     *
     *  This class is usually not used directly. Use BDN_JNI_BEGIN and BDN_JNI_END instead.
     * */
    class JniBlock
    {
    public:
        JniBlock(JNIEnv* pEnv)
        {
            Env::get().jniBlockBegun(pEnv);
        }

        ~JniBlock()
        {
            if(!_ended)
                Env::get().jniBlockEnded();
        }

        void endedWithException(const std::exception& e)
        {
            Env::get().jniBlockEndedWithException(e);
            _ended = true;
        }

        void endedNormally()
        {
            Env::get().jniBlockEnded();
            _ended = true;
        }

    private:
        bool _ended = false;
    };


    /** Indicates that a block of native code was begun.
     *
     *  This is called automatically by the BDN_JNI_BEGIN macro. It is usually not necessary
     *  to call this directly.
     *
     *  pEnv must be a pointer to the JNI environment object that was received by the
     *  JNI function.*/
    void jniBlockBegun(JNIEnv* pEnv)
    {
        _pEnv = pEnv;
    }

    /** Indicates that a JNI block has ended with an exception.
     *
     *  The exception will be forwarded to the Java side.
     *
     *  This is called automatically by the BDN_JNI_END macro. It is usually not necessary
     *  to call this directly.
     *
     * */
    void jniBlockEndedWithException(const std::exception& e)
    {
        setJavaSideException(e);

        jniBlockEnded();
    }


    /** Indicates that a JNI block has ended normally (without an exception).
     *
     *  This is called automatically by the BDN_JNI_END macro. It is usually not necessary
     *  to call this directly.
     * */
    void jniBlockEnded()
    {
        _pEnv = nullptr;
    }



    /** Returns the Jni environment object.
     *  Throws a JniEnvNotSetError if it has not been set yet.
     *  The JNI environment is set with the macro BDN_JNI_BEGIN.
     **/
    JNIEnv* getJniEnv()
    {
        ensureHaveEnv();
        return _pEnv;
    }



    /** If the last JAVA call produced an exception then this is thrown as a C++ exception.*/
    void throwExceptionFromLastJavaCall()
    {
        ensureHaveEnv();

        jthrowable exc = _pEnv->ExceptionOccurred();
        if(exc!=nullptr)
            throw JavaException( JThrowable( LocalReference((jobject)exc) ) );
    }

    void setJavaSideException(const std::exception& e)
    {
        ensureHaveEnv();

        const JavaException* pJavaException = dynamic_cast<const JavaException*>(&e);
        if(pJavaException!=nullptr)
        {
            // the exception already wraps a java exception. Use the wrapped
            // java exception on the java side.
            _pEnv->Throw( (jthrowable)const_cast<JavaException*>(pJavaException)->getJThrowable_().getRef_().getJObject() );
        }
        else
        {
            // this is a C++ exception. Generate a corresponding java exception.
            JException javaException( "Error from JNI component: "+String(e.what()) );

            _pEnv->Throw( (jthrowable)javaException.getRef_().getJObject() );
        }
    }

private:
    void ensureHaveEnv()
    {
        if(_pEnv==nullptr)
            throw JniEnvNotSetError();
    }

    JNIEnv* _pEnv = nullptr;
};

}
}

#endif


