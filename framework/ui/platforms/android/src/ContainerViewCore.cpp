
#include <bdn/android/ContainerViewCore.h>
#include <bdn/entry.h>

namespace bdn
{
    namespace android
    {
        ContainerViewCore::ContainerViewCore(std::shared_ptr<ContainerView> outer)
            : ViewCore(outer, createAndroidViewClass<JNativeViewGroup>(outer))
        {}

        ContainerViewCore::~ContainerViewCore() {}

        void ContainerViewCore::dispose()
        {
            _keepMeAlive.reset();
            ViewCore::dispose();
        }

        double ContainerViewCore::getUIScaleFactor() const { return ViewCore::getUIScaleFactor(); }

        void ContainerViewCore::addChildJView(JView childJView)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());

            parentGroup.addView(childJView);
        }

        void ContainerViewCore::removeChildJView(JView childJView)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());
            parentGroup.removeView(childJView);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeViewGroup_doLayout(JNIEnv *env, jobject rawSelf,
                                                                                 bool changed, int left, int top,
                                                                                 int right, int bottom)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto viewCore =
                    bdn::android::getViewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawSelf))) {
                if (auto view = viewCore->getOuterViewIfStillAttached()) {
                    if (auto layout = view->getLayout()) {
                        layout->layout(view.get());
                    }
                }
            }
        },
        true, env);
}
