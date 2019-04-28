
#include <bdn/java/wrapper/NativeStrongPointer.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

#include <utility>

extern "C" JNIEXPORT void JNICALL Java_io_boden_java_NativeStrongPointer_disposed(JNIEnv *pEnv, jobject rawSelf,
                                                                                  jobject rawByteBuffer)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::java::wrapper::ByteBuffer byteBuffer((bdn::java::Reference::convertExternalLocal(rawByteBuffer)));

            auto *pPtr = static_cast<std::shared_ptr<void> *>(byteBuffer.getBuffer_());

            delete pPtr; // NOLINT(cppcoreguidelines-owning-memory)
        },
        true, pEnv);
}

namespace bdn::java::wrapper
{
    Reference NativeStrongPointer::newInstance_(const std::shared_ptr<void> &pObject)
    {
        if (pObject == nullptr) {
            // When the C++ pointer is null then we just return a null
            // java reference
            return Reference();
        }
        // wrap the pointer into a java byte buffer
        auto *pPtr = new std::shared_ptr<void>(pObject); // NOLINT(cppcoreguidelines-owning-memory)
        ByteBuffer byteBuffer(static_cast<void *>(pPtr), 1);

        static MethodId constructorId;

        Reference ref = getStaticClass_().newInstance_(constructorId, byteBuffer);

        return ref;
    }

    NativeStrongPointer::NativeStrongPointer(const std::shared_ptr<void> &pObject) : Object(newInstance_(pObject)) {}

    NativeStrongPointer::NativeStrongPointer(const Reference &objectRef) : Object(objectRef) {}

    ByteBuffer NativeStrongPointer::getWrappedPointer()
    {
        static MethodId methodId;

        return invoke_<ByteBuffer>(getStaticClass_(), methodId, "getWrappedPointer");
    }

    std::shared_ptr<void> NativeStrongPointer::getPointer()
    {
        ByteBuffer buffer = getWrappedPointer();

        if (buffer.isNull_()) {
            // that means that the C++ pointer is null.
            return nullptr;
        }
        auto *ptr = static_cast<std::shared_ptr<void> *>(buffer.getBuffer_());

        return *ptr;
    }

    /*Base *NativeStrongPointer::unwrapJObject(jobject obj)
    {
        Env &env = Env::get();

        JNIEnv *pEnv = env.getJniEnv();

        if (pEnv->IsSameObject(obj, nullptr) != 0u) {
            return nullptr;
        }
        void *pBuffer = env.getJniEnv()->GetDirectBufferAddress(obj);

        return static_cast<std::shared_ptr<Base> *>(pBuffer)->get();
    }*/

    Class &NativeStrongPointer::getStaticClass_()
    {
        static Class cls("io/boden/java/NativeStrongPointer");

        return cls;
    }
}
