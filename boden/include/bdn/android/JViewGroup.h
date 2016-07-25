#ifndef BDN_ANDROID_JViewGroup_H_
#define BDN_ANDROID_JViewGroup_H_

#include <bdn/android/JView.h>

namespace bdn
{
namespace android
{

/** Wrapper for Java android.view.ViewGroup objects.*/
class JViewGroup : public JView
{
public:
    /** @param javaRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JViewGroup(const bdn::java::Reference& javaRef)
        : JView(javaRef)
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
    static bdn::java::JClass& getStaticClass()
    {
        static bdn::java::JClass cls( "android/view/ViewGroup" );

        return cls;
    }


    void addView(JView & child)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass(), methodId, "addView", child);
    }



};


}
}


#endif


