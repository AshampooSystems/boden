#ifndef BDN_ANDROID_JNativeScrollViewManager_H_
#define BDN_ANDROID_JNativeScrollViewManager_H_

#include <bdn/android/JNativeViewGroup.h>

namespace bdn
{
namespace android
{


/** Accessor for io.boden.android.NativeScrollViewManager objects.*/
class JNativeScrollViewManager : public bdn::java::JObject
{
private:
    static bdn::java::Reference newInstance_(JContext& context)
    {
        static bdn::java::MethodId constructorId;

        return getStaticClass_().newInstance_(constructorId, context);
    }

public:
    JNativeScrollViewManager(JContext& context)
            : JObject( newInstance_(context) )
    {
    }

    /** @param javaRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JNativeScrollViewManager(const bdn::java::Reference& javaRef)
     : bdn::java::JObject( javaRef )
    {
    }




    /** Returns the outermost view object that wraps the whole scroll view.
     *
     *  This view can contain several nested sub views. Use getContentParent()
     *  to get the view that acts as the parent for the content view.
     * */
    JView getWrapperView()
    {
        static bdn::java::MethodId methodId;

        return invoke_<JView>(getStaticClass_(), methodId, "getWrapperView" );
    }


    /** Returns the view group that is the parent of the scroll view's content view.
     *  This NativeViewGroup acts as the glue between our layout system and the scroll view's
     *  internal workings from android.*/
    JNativeViewGroup getContentParent()
    {
        static bdn::java::MethodId methodId;

        return invoke_<JNativeViewGroup>(getStaticClass_(), methodId, "getContentParent" );
    }


    /** Scrolls to the specified scroll position (without animation)*/
    void scrollTo (int x, int y)
    {
        static bdn::java::MethodId methodId;

        return invoke_<void>(getStaticClass_(), methodId, "scrollTo", x, y );
    }

    /** Scrolls to the specified scroll position with a smooth animation*/
    void smoothScrollTo (int x, int y)
    {
        static bdn::java::MethodId methodId;

        return invoke_<void>(getStaticClass_(), methodId, "smoothScrollTo", x, y );
    }


    /** Returns the X coordinate of the scroll position.*/
    int getScrollX()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getScrollX" );
    }

    /** Returns the Y coordinate of the scroll position.*/
    int getScrollY()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getScrollY" );
    }


    /** Returns the width of the scroll view.*/
    int getWidth()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getWidth" );
    }

    /** Returns the height of the scroll view.*/
    int getHeight()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getHeight" );
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
        static bdn::java::JClass cls( "io/boden/android/NativeScrollViewManager" );

        return cls;
    }

    bdn::java::JClass& getClass_() override
    {
        return getStaticClass_();
    }


};


}
}


#endif


