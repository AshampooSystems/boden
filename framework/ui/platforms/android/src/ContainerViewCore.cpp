
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

        void ContainerViewCore::dispose() { ViewCore::dispose(); }

        double ContainerViewCore::getUIScaleFactor() const { return ViewCore::getUIScaleFactor(); }

        void ContainerViewCore::addChildCore(ViewCore *child)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());

            parentGroup.addView(child->getJView());
        }

        void ContainerViewCore::removeChildCore(ViewCore *child)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());
            parentGroup.removeView(child->getJView());
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
