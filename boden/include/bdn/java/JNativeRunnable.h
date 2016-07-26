#ifndef BDN_JAVA_JNativeRunnable_H_
#define BDN_JAVA_JNativeRunnable_H_

#include <bdn/java/JObject.h>
#include <bdn/java/JNativeStrongPointer.h>

#include <bdn/ISimpleCallable.h>

#include <functional>

namespace bdn
{
namespace java
{


/** Accessor for Java-side io.boden.java.NativeRunnable objects.
 *
 *  These objects provide a way for a piece of native code to be executed by a Java-side runnable object.
 *
 * */
class JNativeRunnable : public JObject
{
private:
    class Callable : public Base, BDN_IMPLEMENTS ISimpleCallable
    {
    public:
        Callable(std::function<void()> func)
        {
            _func = func;
        }

        void call() override
        {
            _func();
        }

    private:
        std::function<void()> _func;
    };


    static Reference newInstance_(std::function<void()> func)
    {
        P<Callable> pCallable = newObj<Callable>(func);

        return JNativeStrongPointer( static_cast<IBase*>( static_cast<ISimpleCallable*>( pCallable.getPtr() ) ) ).getRef_();
    }


public:
    explicit JNativeRunnable( std::function<void()> func)
     : JObject( newInstance_(func) )
    {
    }

    /** @param objectRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JNativeRunnable(const Reference& objectRef)
    : JObject(objectRef)
    {
    }


    /** Returns the JClass object for this class.
     *
     *  Note that the returned class object is not necessarily unique for the whole
     *  process.
     *  You might get different objects if this function is called from different
     *  shared libraries.
     *
     *  If you want to check for type equality then you should compare the type name
     *  (see getTypeName() )
     *  */
    static JClass& getStaticClass()
    {
        static JClass cls( "io/boden/java/NativeRunnable" );

        return cls;
    }

    JClass& getClass_() override
    {
        return getStaticClass_();
    }


};


}
}


#endif


