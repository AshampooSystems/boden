#include <bdn/android/SliderCore.h>
#include <bdn/android/wrapper/NativeSeekBar.h>

#include <bdn/entry.h>

namespace bdn::detail
{
    CORE_REGISTER(Slider, bdn::android::SliderCore, Slider)
}

namespace bdn::android
{
    SliderCore::SliderCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::NativeSeekBar>(viewCoreFactory))
    {
        getJViewAS<wrapper::SeekBar>().setMin(0);
        getJViewAS<wrapper::SeekBar>().setMax(1000);

        value.onChange() += [=](const auto &p) { getJViewAS<wrapper::SeekBar>().setProgress(p.get() * 1000); };
    }

    void SliderCore::valueChanged(int newValue) { value = (double)newValue / 1000.0; }
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeSeekBar_native_1valueChanged(JNIEnv *env, jobject rawSelf,
                                                                                           int newValue)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core =
                    bdn::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawSelf))) {
                if (auto sliderCore = std::dynamic_pointer_cast<bdn::android::SliderCore>(core)) {
                    sliderCore->valueChanged(newValue);
                }
            }
        },
        true, env);
}
