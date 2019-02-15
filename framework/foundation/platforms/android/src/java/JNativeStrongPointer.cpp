
#include <bdn/java/JNativeStrongPointer.h>

#include <bdn/Base.h>
#include <bdn/entry.h>
#include <bdn/java/Env.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_java_NativeStrongPointer_disposed(JNIEnv *pEnv, jobject rawSelf,
                                                                                  jobject rawByteBuffer)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::java::JByteBuffer byteBuffer((bdn::java::Reference::convertExternalLocal(rawByteBuffer)));

            std::shared_ptr<bdn::Base> *pPtr = static_cast<std::shared_ptr<bdn::Base> *>(byteBuffer.getBuffer_());

            delete pPtr;
        },
        true, pEnv);
}

namespace bdn
{
    namespace java
    {
        Reference JNativeStrongPointer::newInstance_(std::shared_ptr<Base> pObject)
        {
            if (pObject == nullptr) {
                // When the C++ pointer is null then we just return a null
                // java reference
                return Reference();
            } else {
                // wrap the pointer into a java byte buffer
                std::shared_ptr<Base> *pPtr = new std::shared_ptr<Base>(pObject);
                JByteBuffer byteBuffer(static_cast<void *>(pPtr), 1);

                static MethodId constructorId;

                Reference ref = getStaticClass_().newInstance_(constructorId, byteBuffer);

                return ref;
            }
        }

        JNativeStrongPointer::JNativeStrongPointer(std::shared_ptr<Base> pObject) : JObject(newInstance_(pObject)) {}

        JNativeStrongPointer::JNativeStrongPointer(const Reference &objectRef) : JObject(objectRef) {}

        JByteBuffer JNativeStrongPointer::getWrappedPointer()
        {
            static MethodId methodId;

            return invoke_<JByteBuffer>(getStaticClass_(), methodId, "getWrappedPointer");
        }

        std::shared_ptr<Base> JNativeStrongPointer::getPointer_()
        {
            JByteBuffer buffer = getWrappedPointer();

            if (buffer.isNull_()) {
                // that means that the C++ pointer is null.
                return nullptr;
            } else {
                std::shared_ptr<Base> *ptr = static_cast<std::shared_ptr<Base> *>(buffer.getBuffer_());

                return *ptr;
            }
        }

        Base *JNativeStrongPointer::unwrapJObject(jobject obj)
        {
            Env &env = Env::get();

            JNIEnv *pEnv = env.getJniEnv();

            if (pEnv->IsSameObject(obj, NULL))
                return nullptr;
            else {
                void *pBuffer = env.getJniEnv()->GetDirectBufferAddress(obj);

                env.throwAndClearExceptionFromLastJavaCall();

                return static_cast<std::shared_ptr<Base> *>(pBuffer)->get();
            }
        }

        JClass &JNativeStrongPointer::getStaticClass_()
        {
            static JClass cls("io/boden/java/NativeStrongPointer");

            return cls;
        }
    }
}
