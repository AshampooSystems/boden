#ifndef BDN_ANDROID_JView_H_
#define BDN_ANDROID_JView_H_

#include <bdn/java/JObject.h>
#include <bdn/android/JContext.h>
#include <bdn/android/JViewParent.h>

namespace bdn
{
namespace android
{


/** Accessor for Java android.view.View objects.*/
class JView : public bdn::java::JObject
{
public:
    /** @param javaRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JView(const bdn::java::Reference& javaRef)
    : JObject(javaRef)
    {
    }

    JView()
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
    static bdn::java::JClass& getStaticClass_()
    {
        static bdn::java::JClass cls( "android/view/View" );

        return cls;
    }

    bdn::java::JClass& getClass_ () override
    {
        return getStaticClass_ ();
    }


    enum Visibility
    {
        visible = 0,

        /** The view is invisible, but still takes up space during layout.*/
        invisible = 4,

        /** The view is invisible and does NOT take up space during layout.*/
        gone = 8
    };

    void setVisibility(Visibility visibility)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setVisibility", (int)visibility);
    }

    Visibility getVisibility()
    {
        static bdn::java::MethodId methodId;

        return (Visibility)invoke_<int>(getStaticClass_(), methodId, "getVisibility");
    }


    int getLeft()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getLeft" );
    }


    int getTop()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getTop" );
    }


    int getWidth()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getWidth" );
    }


    int getHeight()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getHeight" );
    }


    void setPadding(int left,
                    int top,
                    int right,
                    int bottom)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setPadding", left, top, right, bottom );
    }


    int getPaddingLeft()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getPaddingLeft" );
    }

    int getPaddingTop()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getPaddingTop" );
    }

    int getPaddingRight()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getPaddingRight" );
    }

    int getPaddingBottom()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getPaddingBottom" );
    }


    /** Returns the view's parent. The object supports the interface
        android.view.ViewParent */
    JViewParent getParent()
    {
        static bdn::java::MethodId methodId;

        return invoke_<JViewParent>(getStaticClass_(), methodId, "getParent" );
    }


    /** Returns the context the view is running in, through which it can access
     *  the current theme, resources, etc.*/
    JContext getContext()
    {
        static bdn::java::MethodId methodId;

        return invoke_<JContext>(getStaticClass_(), methodId, "getContext" );
    }


    /** Attaches an object to the view as a "tag". Tags have no predefined
     *  meaning, so this is simply a way to associate an object with the view.*/
    void setTag( bdn::java::JObject tagObject)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setTag", tagObject );
    }


    /** Returns a previously attached tag object (see setTag() ). Returns a null reference
     *  if no tag object has been attached.*/
    bdn::java::JObject getTag()
    {
        static bdn::java::MethodId methodId;

        return invoke_<JObject>(getStaticClass_(), methodId, "getTag" );
    }


    void measure(int widthMeasureSpec, int heightMeasureSpec)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "measure", widthMeasureSpec, heightMeasureSpec);
    }

    int getMeasuredWidth()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getMeasuredWidth" );
    }

    int getMeasuredHeight()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getMeasuredHeight" );
    }



    class MeasureSpec
    {
    public:

        enum Mode
        {
            /** Indicates that the view can be as large as it wants up to the specified size.*/
            unspecified = 0,

            /** Indicates that the view must be exactly the given size.
             * */
            exactly = 0x40000000,

            /** Indicates that there is no size constraint on the view.*/
            atMost = 0x80000000
        };

        static int makeMeasureSpec (int size, Mode mode)
        {
            return size | mode;
        }

    };



    void requestLayout ()
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "requestLayout");
    }


    void invalidate()
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "invalidate");
    }

    int getSuggestedMinimumWidth()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getSuggestedMinimumWidth");
    }

    int getSuggestedMinimumHeight()
    {
        static bdn::java::MethodId methodId;

        return invoke_<int>(getStaticClass_(), methodId, "getSuggestedMinimumHeight");
    }


    class OnClickListener : public bdn::java::JObject
    {
    public:
        /** @param javaRef the reference to the Java object.
        *      The JObject instance will copy this reference and keep its type.
        *      So if you want the JObject instance to hold a strong reference
        *      then you need to call toStrong() on the reference first and pass the result.
        *      */
        explicit OnClickListener(const bdn::java::Reference& javaRef)
                : JObject(javaRef)
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
            static bdn::java::JClass cls( "android/view/View$OnClickListener" );

            return cls;
        }

        bdn::java::JClass& getClass_ () override
        {
            return getStaticClass_ ();
        }
    };


    void setOnClickListener( JView::OnClickListener& listener )
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_ (), methodId, "setOnClickListener", listener);
    }



    class OnLayoutChangeListener : public bdn::java::JObject
    {
    public:
        /** @param javaRef the reference to the Java object.
        *      The JObject instance will copy this reference and keep its type.
        *      So if you want the JObject instance to hold a strong reference
        *      then you need to call toStrong() on the reference first and pass the result.
        *      */
        explicit OnLayoutChangeListener(const bdn::java::Reference& javaRef)
                : JObject(javaRef)
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
        static bdn::java::JClass& getStaticClass_()
        {
            static bdn::java::JClass cls( "android/view/View$OnLayoutChangeListener" );

            return cls;
        }

        bdn::java::JClass& getClass_() override
        {
            return getStaticClass_ ();
        }
    };


    void addOnLayoutChangeListener(OnLayoutChangeListener listener)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "addOnLayoutChangeListener", listener );
    }

};


}
}


#endif


