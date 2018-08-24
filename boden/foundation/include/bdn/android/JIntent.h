#ifndef BDN_ANDROID_JIntent_H_
#define BDN_ANDROID_JIntent_H_

#include <bdn/java/JObject.h>
#include <bdn/android/JBundle.h>

namespace bdn
{
namespace android
{


/** Accessor for Java android.content.Intent objects.*/
class JIntent : public bdn::java::JObject
{
public:
    /** @param javaRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JIntent(const bdn::java::Reference& javaRef)
    : JObject(javaRef)
    {
    }

    static String ACTION_MAIN;
    static String ACTION_VIEW;


    /** The general action to be performed, such as ACTION_VIEW, ACTION_EDIT, ACTION_MAIN
     * */
    String getAction()
    {
        static bdn::java::MethodId methodId;

        return invoke_<String>(getStaticClass_(), methodId, "getAction");
    }


    /** Retrieves a map of extended data from the intent. */
    JBundle getExtras()
    {
        static bdn::java::MethodId methodId;

        return invoke_<JBundle>(getStaticClass_(), methodId, "getExtras");
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
        static bdn::java::JClass cls( "android/content/Intent" );

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


