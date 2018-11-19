#include <bdn/init.h>
#include <bdn/java/JNativeStrongPointer.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_java_NativeStrongPointer_disposed(JNIEnv *env, jobject rawSelf,
                                                                                  jobject rawByteBuffer)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::java::JByteBuffer byteBuffer((bdn::java::Reference::convertExternalLocal(rawByteBuffer)));

            bdn::IBase *object = static_cast<bdn::IBase *>(byteBuffer.getBuffer_());

            object->releaseRef();
        },
        true, env);
}

namespace bdn
{
    namespace java
    {
        Reference JNativeStrongPointer::newInstance_(IBase *object)
        {
            if (object == nullptr) {
                // When the C++ pointer is null then we just return a null
                // java reference
                return Reference();
            } else {
                // wrap the pointer into a java byte buffer
                JByteBuffer byteBuffer(static_cast<void *>(object), 1);

                static MethodId constructorId;

                Reference ref = getStaticClass_().newInstance_(constructorId, byteBuffer);

                object->addRef();

                return ref;
            }
        }

        JNativeStrongPointer::JNativeStrongPointer(IBase *object) : JObject(newInstance_(object)) {}

        JNativeStrongPointer::JNativeStrongPointer(const Reference &objectRef) : JObject(objectRef) {}

        JByteBuffer JNativeStrongPointer::getWrappedPointer()
        {
            static MethodId methodId;

            return invoke_<JByteBuffer>(getStaticClass_(), methodId, "getWrappedPointer");
        }

        P<IBase> JNativeStrongPointer::getPointer_()
        {
            JByteBuffer buffer = getWrappedPointer();

            if (buffer.isNull_()) {
                // that means that the C++ pointer is null.
                return nullptr;
            } else {
                IBase *p = static_cast<IBase *>(buffer.getBuffer_());

                return p;
            }
        }

        IBase *JNativeStrongPointer::unwrapJObject(jobject obj)
        {
            Env &env = Env::get();

            JNIEnv *jniEnv = env.getJniEnv();

            if (jniEnv->IsSameObject(obj, NULL))
                return nullptr;
            else {
                void *buffer = env.getJniEnv()->GetDirectBufferAddress(obj);

                env.throwAndClearExceptionFromLastJavaCall();

                return static_cast<IBase *>(buffer);
            }
        }

        JClass &JNativeStrongPointer::getStaticClass_()
        {
            static JClass cls("io/boden/java/NativeStrongPointer");

            return cls;
        }
    }
}
