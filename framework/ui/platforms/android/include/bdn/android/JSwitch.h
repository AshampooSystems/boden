#ifndef BDN_ANDROID_JSwitch_H_
#define BDN_ANDROID_JSwitch_H_

#include <bdn/android/JCompoundButton.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java android.widget.Switch objects.*/
        class JSwitch : public JCompoundButton
        {
          private:
            static bdn::java::Reference newInstance_(JContext &context)
            {
                static bdn::java::MethodId constructorId;

                return getStaticClass_().newInstance_(constructorId, context);
            }

          public:
            JSwitch(JContext &context) : JCompoundButton(newInstance_(context))
            {}

            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JSwitch(const bdn::java::Reference &javaRef)
                : JCompoundButton(javaRef)
            {}

            JSwitch() {}

            void setChecked(bool checked)
            {
                static bdn::java::MethodId methodId;

                invoke_<void>(getStaticClass_(), methodId, "setChecked",
                              checked);
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
                static bdn::java::JClass cls("android/widget/Switch");

                return cls;
            }
        };
    }
}

#endif
