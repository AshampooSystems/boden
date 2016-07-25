#ifndef BDN_ANDROID_JButton_H_
#define BDN_ANDROID_JButton_H_

#include <bdn/android/JTextView.h>

namespace bdn
{
namespace android
{


/** Accessor for Java android.widget.Button objects.*/
class JButton : public JTextView
{
public:
    JButton()
     : JTextView( getStaticClass_ ()->newInstance().toStrong() )
    {
    }

    /** @param javaRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JButton(const bdn::java::Reference& javaRef)
     : JTextView(javaRef)
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
        static bdn::java::JClass cls( "android/widget/Button" );

        return cls;
    }

    void setText(const String& text)
    {
        static bdn::java::MethodId methodId;

        invoke_<void>(getStaticClass_(), methodId, "setText", JCharSequence( JString(text).getJavaReference() ) );
    }



};


}
}


#endif


