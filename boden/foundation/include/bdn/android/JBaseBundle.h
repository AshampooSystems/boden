#ifndef BDN_ANDROID_JBaseBundle_H_
#define BDN_ANDROID_JBaseBundle_H_

#include <bdn/java/JObject.h>
#include <bdn/java/JString.h>
#include <bdn/java/ArrayOfObjects.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java android.os.BaseBundle objects.*/
        class JBaseBundle : public bdn::java::JObject
        {
          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JBaseBundle(const bdn::java::Reference &javaRef)
                : JObject(javaRef)
            {}

            /** Returns the value associated with the given key, or null if no
             * mapping of the desired type exists for the given key or a null
             * value is explicitly associated with the key.
             * */
            bdn::java::JString getString(const String &name)
            {
                static bdn::java::MethodId methodId;

                return invoke_<bdn::java::JString>(getStaticClass_(), methodId,
                                                   "getString", name);
            }

            /** Returns the value associated with the given key, or null if no
             * mapping of the desired type exists for the given key or a null
             * value is explicitly associated with the key.
             * */
            bdn::java::ArrayOfObjects<bdn::java::JString>
            getStringArray(const String &name)
            {
                static bdn::java::MethodId methodId;

                return invoke_<bdn::java::ArrayOfObjects<bdn::java::JString>>(
                    getStaticClass_(), methodId, "getStringArray", name);
            }

            /** Returns the JClass object for this class.
             *
             *  Note that the returned class object is not necessarily unique
             * for the whole process. You might get different objects if this
             * function is called from different shared libraries.
             *
             *  If you want to check for type equality then you should compare
             * the type name (see getTypeName() )
             *  */
            static bdn::java::JClass &getStaticClass_()
            {
                static bdn::java::JClass cls("android/os/BaseBundle");

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
