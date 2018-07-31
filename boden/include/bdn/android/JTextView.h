#ifndef BDN_ANDROID_JTextView_H_
#define BDN_ANDROID_JTextView_H_

#include <bdn/android/JView.h>
#include <bdn/android/JContext.h>
#include <bdn/android/JTextPaint.h>
#include <bdn/android/JTextWatcher.h>
#include <bdn/java/JString.h>

namespace bdn
{
namespace android
{


/** Accessor for Java android.widget.TextView objects.*/
class JTextView : public JView
{
private:
    static bdn::java::Reference newInstance_(JContext context)
    {
        static bdn::java::MethodId constructorId;

        return getStaticClass_().newInstance_(constructorId, context);
    }

public:
    /** @param objectRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JTextView(const bdn::java::Reference& objectRef)
        : JView(objectRef)
    {
    }


    JTextView( JContext context )
    : JTextView( newInstance_(context) )
    {
    }


    JTextView()
    {
    }


    /** Returns the TextPaint object that is used by this text view.*/
    JTextPaint getPaint()
    {
        static bdn::java::MethodId methodId;

        return invoke_<JTextPaint>(getStaticClass_(), methodId, "getPaint");
    }



    void setText(const String& text)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setText", bdn::java::JCharSequence( bdn::java::JString(text).getRef_() ) );
    }


    String getText()
    {
        static bdn::java::MethodId methodId;

        return invoke_<bdn::java::JCharSequence>(getStaticClass_(), methodId, "getText" ).toString();
    }


    void setSingleLine(bool singleLine)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setSingleLine", singleLine);
    }


    enum class BreakStrategy
    {
        /** Value for break strategy indicating simple line breaking. Automatic hyphens are not added
         *  (though soft hyphens are respected), and modifying text generally doesn't affect the layout
         *  before it (which yields a more consistent user experience when editing), but layout may not
         *  be the highest quality.*/
        simple = 0,

        /** Value for break strategy indicating high quality line breaking, including automatic
         *  hyphenation and doing whole-paragraph optimization of line breaks.*/
        highQuality = 1,

        /** Value for break strategy indicating balanced line breaking. The breaks are chosen
         *  to make all lines as close to the same length as possible, including automatic hyphenation.*/
        balanced = 2
    };

    /** Sets the break strategy for breaking paragraphs into lines.
     *  The default value for TextView is BREAK_STRATEGY_HIGH_QUALITY,
     *  and the default value for EditText is BREAK_STRATEGY_SIMPLE, the latter
     *  to avoid the text "dancing" when being edited.*/
    void setBreakStrategy(BreakStrategy strategy)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setBreakStrategy", (int)strategy);
    }



    /** Sets whether the text should be allowed to be wider than the View is. If false, it will
     *  be wrapped to the width of the View.*/
    void setHorizontallyScrolling(bool scroll)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setHorizontallyScrolling", scroll);
    }



    /** Makes the TextView at most this many lines tall. Setting this value overrides any other (maximum) height setting. */
    void setMaxLines(int lines)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setMaxLines", lines);
    }


    /** Makes the TextView exactly this many pixels wide. You could do the same thing by specifying this number in the LayoutParams. */
    void setWidth(int pixels)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setWidth", pixels);
    }




    /** Makes the TextView at most this many pixels wide */
    void setMaxWidth(int maxPixels)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setMaxWidth", maxPixels);
    }


    /** Makes the TextView at least this many pixels tall. Setting this value overrides any other (minimum) number of lines setting.*/
    void setMaxHeight(int maxPixels)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setMaxHeight", maxPixels);
    }


    /** Returns the size (in pixels) of the default text size in this TextView.*/
    double getTextSize()
    {
        static bdn::java::MethodId methodId;

        return invoke_<float>(getStaticClass_(), methodId, "getTextSize");
    }



    /** Sets the text appearance from the specified style resource.
    */
    void setTextAppearance (int resId)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setTextAppearance", resId);
    }


    /** Adds a text watcher to observe changes to editable text, see JEditText. */
    void addTextChangedListener(JTextWatcher& watcher)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_ (), methodId, "addTextChangedListener", watcher);
    }

    void removeTextChangedListener(JTextWatcher& watcher)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_ (), methodId, "removeTextChangedListener", watcher);        
    }


    class OnEditorActionListener : public bdn::java::JObject
    {
    public:
        /** @param javaRef the reference to the Java object.
        *      The JObject instance will copy this reference and keep its type.
        *      So if you want the JObject instance to hold a strong reference
        *      then you need to call toStrong() on the reference first and pass the result.
        *      */
        explicit OnEditorActionListener(const bdn::java::Reference& javaRef)
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
            static bdn::java::JClass cls( "android/widget/TextView$OnEditorActionListener" );

            return cls;
        }

        bdn::java::JClass& getClass_ () override
        {
            return getStaticClass_ ();
        }
    };

    /** Adds a text watcher to observe changes to editable text, see JEditText. */
    void setOnEditorActionListener(OnEditorActionListener& listener)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_ (), methodId, "setOnEditorActionListener", listener);
    }


    /** Returns the JClass object for this class.s
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
        static bdn::java::JClass cls( "android/widget/TextView" );

        return cls;
    }

    bdn::java::JClass& getClass_()
    {
        return getStaticClass_();
    }


};


}
}


#endif


