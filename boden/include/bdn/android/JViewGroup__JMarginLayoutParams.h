#ifndef BDN_ANDROID_JViewGroup__JMarginLayoutParams_H_
#define BDN_ANDROID_JViewGroup__JMarginLayoutParams_H_

#include <bdn/android/JViewGroup__JLayoutParams.h>

namespace bdn
{
namespace android
{

/** Wrapper for Java android.view.ViewGroup.MarginLayoutParams objects.
 *
 *  This is defined outside of JViewGroup for technical reasons. JViewGroup
 *  also has an alias to this class; it can also be accessed under the name JViewGroup::JLayoutParams
 * */
class JViewGroup__JMarginLayoutParams : public JViewGroup__JLayoutParams
{
private:
    static bdn::java::Reference newInstance_(int width, int height)
    {
        static bdn::java::MethodId constructorId;

        return getStaticClass_().newInstance_(constructorId, width, height);
    }

public:
    /** @param javaRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JViewGroup__JMarginLayoutParams(const bdn::java::Reference &javaRef)
            : JViewGroup__JLayoutParams(javaRef)
    {
    }


    JViewGroup__JMarginLayoutParams(int width, int height)
            : JViewGroup__JLayoutParams( newInstance_(width, height) )
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
    static bdn::java::JClass &getStaticClass_()
    {
        static bdn::java::JClass cls("android/view/ViewGroup$MarginLayoutParams");

        return cls;
    }

    bdn::java::JClass &getClass_() override
    {
        return getStaticClass_();
    }

};


}
}


#endif


