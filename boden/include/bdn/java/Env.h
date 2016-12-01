#ifndef BDN_JAVA_Env_H_
#define BDN_JAVA_Env_H_

#include <bdn/java/JniEnvNotSetError.h>
#include <bdn/java/JavaException.h>
#include <bdn/java/JException.h>

#include <jni.h>



namespace bdn
{
namespace java
{


/** Used to access the java environment. Wraps a JNIEnv instance.
 *
 *  Each Env object can only be used from a single thread. Because of this it is discouraged
 *  to store pointers to Env objects for later use.
 *
 *  Instead you can use the static get() function to access the object for the current thread.
 *
 */
class Env : public Base
{
public:
    ~Env();

    /** Static function that returns a reference to the Env instance for the current thread. The instance cannot be used
     *  in other threads.
     *  */
    BDN_SAFE_STATIC_THREAD_LOCAL(Env, get );



    /** Indicates that a block of native code was begun.
     *
     *  This is called automatically by the BDN_ENTRY_BEGIN macro. It is usually not necessary
     *  to call this directly.
     *
     *  pEnv must be a pointer to the JNI environment object that was received by the
     *  JNI function.*/
    void jniBlockBegun(JNIEnv* pEnv)
    {
        // note that it is safe to cache the pointer since the Env objects are all
        // thread local (and the JNIEnv stays the same for the same thread).
        if(_pEnvIfKnown==nullptr)
            setEnv(pEnv, false);
    }


    /** Indicates that a JNI block has ended normally (without an exception).
     *
     *  This is called automatically by the BDN_JNI_END macro. It is usually not necessary
     *  to call this directly.
     * */
    void jniBlockEnded()
    {
    }



    /** Returns the Jni environment object.
     *  Throws a JniEnvNotSetError if it has not been set yet.
     *  The JNI environment is set with the macro BDN_ENTRY_BEGIN.
     **/
    JNIEnv* getJniEnv()
    {
        if(_pEnvIfKnown==nullptr)
            createEnv();
        return _pEnvIfKnown;
    }



    /** If the last JAVA call produced an exception then this is thrown as a C++ exception
     *  and the exception is cleared from the global state. Since the exception is cleared,
     *  a second call to throwAndClearExceptionFromLastJavaCall() will return without any exception.
     * */
    void throwAndClearExceptionFromLastJavaCall()
    {
        JNIEnv* pEnv = getJniEnv();

        jthrowable exc = pEnv->ExceptionOccurred();
        pEnv->ExceptionClear();
        if(exc!=nullptr)
            throw JavaException( JThrowable( Reference::convertAndDestroyOwnedLocal((jobject)exc) ) );
    }


    void setJavaSideException(const std::exception_ptr& exceptionPtr)
    {
        JNIEnv* pEnv = getJniEnv();

        try
        {
            std::rethrow_exception(exceptionPtr);
        }
        catch(JavaException& e)
        {
            // the exception already wraps a java exception. Use the wrapped
            // java exception on the java side.
            pEnv->Throw( (jthrowable)e.getJThrowable_().getRef_().getJObject() );
        }
        catch(std::exception& e)
        {
            // this is a C++ exception. Generate a corresponding java exception.
            JException javaException("Exception from JNI component: " + String(e.what()));
            pEnv->Throw((jthrowable) javaException.getRef_().getJObject());
        }
        catch(...)
        {
            JException javaException("Exception of unknown type from JNI component.");
            pEnv->Throw((jthrowable) javaException.getRef_().getJObject());
        }
    }


    /** Used internally. Do not call.*/
    static void _onLoad(JavaVM* pVm);

private:
    Env();
    friend class RawNewAllocator_Base_<Env>;

    void createEnv();
    void setEnv(JNIEnv* pEnv, bool mustDetachThread);


    JNIEnv* _pEnvIfKnown;
    bool    _mustDetachThread;

    static JavaVM* _pGlobalVm;
};

}
}

#endif


