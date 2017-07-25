#ifndef BDN_ANDROID_JViewGroup__JLayoutParams_H_
#define BDN_ANDROID_JViewGroup__JLayoutParams_H_

#include <bdn/java/JObject.h>

namespace bdn
{
namespace android
{

/** Wrapper for Java android.view.ViewGroup.LayoutParams objects.
 *
 *  This is defined outside of JViewGroup for technical reasons. JViewGroup
 *  also has an alias to this class; it can also be accessed under the name JViewGroup::JLayoutParams
 * */
class JViewGroup__JLayoutParams : public bdn::java::JObject
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
    explicit JViewGroup__JLayoutParams(const bdn::java::Reference &javaRef)
            : bdn::java::JObject(javaRef)
    {
    }


    JViewGroup__JLayoutParams(int width, int height)
            : bdn::java::JObject( newInstance_(width, height) )
    {
    }


    enum
    {
        /** Special value for the height or width requested by a View. MATCH_PARENT means that
         *  the view wants to be as big as its parent, minus the parent's padding, if any. Introduced in API Level 8.*/
        MATCH_PARENT = -1,

        /** Special value for the height or width requested by a View. WRAP_CONTENT means that
         the view wants to be just large enough to fit its own internal content, taking its own
         padding into account.*/
        WRAP_CONTENT = -2
    };


    /** Information about how tall the view wants to be. Can be one of the constants FILL_PARENT
     * (replaced by MATCH_PARENT in API Level 8) or WRAP_CONTENT, or an exact size.
     * */
    bdn::java::ObjectField<int> width()
    {
        static bdn::java::ObjectField<int>::Id fieldId( getStaticClass_(), "width");

        return bdn::java::ObjectField<int>( getRef_(), fieldId);
    }

    /** Information about how wide the view wants to be. Can be one of the constants FILL_PARENT
     *  (replaced by MATCH_PARENT in API Level 8) or WRAP_CONTENT, or an exact size.
     * */
    bdn::java::ObjectField<int> height()
    {
        static bdn::java::ObjectField<int>::Id fieldId( getStaticClass_(), "height");

        return bdn::java::ObjectField<int>( getRef_(), fieldId);
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
        static bdn::java::JClass cls("android/view/ViewGroup$LayoutParams");

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


