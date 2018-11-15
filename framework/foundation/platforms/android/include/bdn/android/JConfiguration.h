#ifndef BDN_ANDROID_JConfiguration_H_
#define BDN_ANDROID_JConfiguration_H_

#include <bdn/java/JObject.h>
#include <bdn/java/ObjectField.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java-side android.content.res.Configuration objects.*/
        class JConfiguration : public bdn::java::JObject
        {
          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JConfiguration(const bdn::java::Reference &javaRef) : JObject(javaRef) {}

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
                static bdn::java::JClass cls("android/content/res/Configuration");

                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }

            enum
            {
                DENSITY_DPI_UNDEFINED = 0
            };

            /** The target screen density being rendered to, corresponding to
             * density resource qualifier. Set to DENSITY_DPI_UNDEFINED if no
             * density is specified.*/
            bdn::java::ObjectField<int> densityDpi()
            {
                static bdn::java::ObjectField<int>::Id fieldId(getStaticClass_(), "densityDpi");

                return bdn::java::ObjectField<int>(getRef_(), fieldId);
            }
        };
    }
}

#endif
