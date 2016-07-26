#ifndef BDN_JAVA_JString_H_
#define BDN_JAVA_JString_H_

#include <bdn/java/JCharSequence.h>
#include <bdn/java/Env.h>

namespace bdn
{
namespace java
{


/** Wrapper for Java java.lang.String objects.*/
class JString : public JCharSequence
{
private:
    static Reference newInstance_ (const String& s)
    {
        Env& env = Env::get();

        jstring obj = env.getJniEnv()->NewStringUTF( s.asUtf8Ptr() );
        env.throwExceptionFromLastJavaCall();

        return LocalReference( (jobject)obj );
    }

public:
    /** @param objectRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JString(const Reference& objectRef)
    : JCharSequence(objectRef)
    {
    }

    JString(const String& inString)
     : JString( newInstance_(inString) )
    {
    }


    String getValue_()
    {
        JNIEnv* pEnv = Env::get().getJniEnv();
        jstring javaRef = (jstring)getJObject_();

        const char* data = pEnv->GetStringUTFChars(javaRef , nullptr );

        String val(data);

        pEnv->ReleaseStringUTFChars( javaRef, data );

        return val;
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
    static JClass& getStaticClass_ ()
    {
        static JClass cls( "java/lang/String" );

        return cls;
    }

    JClass& getClass_ () override
    {
        return getStaticClass_ ();
    }




};


}
}


#endif


