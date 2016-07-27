#ifndef BDN_ANDROID_JNativeHandler_H_
#define BDN_ANDROID_JNativeHandler_H_

#include <bdn/android/JHandler.h>
#include <bdn/android/JLooper.h>

namespace bdn
{
namespace android
{


/** Accessor for Java io.boden.android.NativeHandler class.
 *
 *  There is a static singleton of NativeHandler on the java side that can be accessed
 *  via getMainNativeHandler().
 * */
class JNativeHandler : public JHandler
{
public:
    /** @param javaRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JNativeHandler(const bdn::java::Reference& javaRef)
    : JHandler(javaRef)
    {
    }


    static JNativeHandler getMainNativeHandler()
    {
        static bdn::java::MethodId methodId;

        return invokeStatic_<JNativeHandler>(getStaticClass_(), methodId, "getMainNativeHandler" );
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
    static bdn::java::JClass& getStaticClass_()
    {
        static bdn::java::JClass cls( "io/boden/android/NativeHandler" );

        return cls;
    }

    bdn::java::JClass& getClass_ () override
    {
        return getStaticClass_ ();
    }



};


}
}


#endif


