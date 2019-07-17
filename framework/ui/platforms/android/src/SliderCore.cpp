#include <bdn/android/SliderCore.h>
#include <bdn/android/wrapper/NativeSeekBar.h>

#include <bdn/entry.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(Slider, bdn::ui::android::SliderCore, Slider)
}

namespace bdn::ui::android
{
    SliderCore::SliderCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::NativeSeekBar>(viewCoreFactory))
    {
        getJViewAS<bdn::android::wrapper::NativeSeekBar>().setMin(0);
        getJViewAS<bdn::android::wrapper::NativeSeekBar>().setMax(1000);

        value.onChange() +=
            [=](const auto &p) { getJViewAS<bdn::android::wrapper::NativeSeekBar>().setProgress(p.get() * 1000); };
    }

    void SliderCore::valueChanged(int newValue) { value = (double)newValue / 1000.0; }
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeSeekBar_native_1valueChanged(JNIEnv *env, jobject rawSelf,
                                                                                           int newValue)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core =
                    bdn::ui::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawSelf))) {
                if (auto sliderCore = std::dynamic_pointer_cast<bdn::ui::android::SliderCore>(core)) {
                    sliderCore->valueChanged(newValue);
                }
            }
        },
        true, env);
}
