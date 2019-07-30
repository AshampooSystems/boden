#include <bdn/android/ImageViewCore.h>
#include <bdn/android/wrapper/NativeImageView.h>

#include <bdn/entry.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(ImageView, bdn::ui::android::ImageViewCore, ImageView)
}

namespace bdn::ui::android
{
    ImageViewCore::ImageViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::NativeImageView>(viewCoreFactory))
    {
        url.onChange() += [=](auto &property) {
            _imageSize = Size{0, 0};
            getJViewAS<bdn::android::wrapper::NativeImageView>().loadUrl(property.get());
        };
    }

    void ImageViewCore::imageLoaded(int width, int height) { _imageSize = Size{(double)width, (double)height}; }

    Size ImageViewCore::sizeForSpace(Size availableSpace) const
    {
        Size result = _imageSize;

        if (availableSpace.width > availableSpace.height) {
            float factor = result.width / result.height;
            result.width = availableSpace.height * factor;
            result.height = availableSpace.height;
        } else {
            float factor = result.height / result.width;
            result.width = availableSpace.width;
            result.height = availableSpace.width * factor;
        }
        return result;
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeImageView_native_1imageLoaded(JNIEnv *env,
                                                                                            jobject rawSelf, int width,
                                                                                            int height)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core =
                    bdn::ui::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawSelf))) {
                if (auto imageCore = std::dynamic_pointer_cast<bdn::ui::android::ImageViewCore>(core)) {
                    imageCore->imageLoaded(width, height);
                }
            }
        },
        true, env);
}
