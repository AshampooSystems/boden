#ifndef BDN_ANDROID_JKeyEvent_H_
#define BDN_ANDROID_JKeyEvent_H_

#include <bdn/android/JInputEvent.h>

namespace bdn
{
namespace android
{

class JKeyEvent : public JInputEvent
{
public:
    /** @param javaRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JKeyEvent(const bdn::java::Reference& javaRef)
        : JInputEvent(javaRef)
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
    static bdn::java::JClass& getStaticClass_ ()
    {
        static bdn::java::JClass cls( "android/view/KeyEvent" );

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