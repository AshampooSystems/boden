
#include <bdn/android/ContainerViewCore.h>
#include <bdn/entry.h>

namespace bdn::android
{
    ContainerViewCore::ContainerViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, createAndroidViewClass<wrapper::NativeViewGroup>(uiProvider))
    {}

    ContainerViewCore::~ContainerViewCore() = default;

    double ContainerViewCore::getUIScaleFactor() const { return ViewCore::getUIScaleFactor(); }

    void ContainerViewCore::addChildView(std::shared_ptr<View> child)
    {
        if (auto childCore = child->core<android::ViewCore>()) {
            _children.push_back(child);
            auto group = getJViewAS<wrapper::NativeViewGroup>();
            group.addView(childCore->getJView());
        } else {
            throw std::runtime_error("Tried adding Child with incompatible core");
        }

        scheduleLayout();
        updateChildren();
    }

    void ContainerViewCore::removeChildView(std::shared_ptr<View> child)
    {
        if (auto childCore = child->core<android::ViewCore>()) {
            _children.remove(child);
            auto group = getJViewAS<wrapper::NativeViewGroup>();
            group.removeView(childCore->getJView());
        } else {
            throw std::runtime_error("Tried removing Child with incompatible core");
        }

        scheduleLayout();
    }

    std::list<std::shared_ptr<bdn::View>> ContainerViewCore::childViews() { return _children; }

    void ContainerViewCore::visitInternalChildren(const std::function<void(std::shared_ptr<bdn::ViewCore>)> &function)
    {
        for (auto &child : _children) {
            if (auto childCore = child->core<bdn::ViewCore>()) {
                function(childCore);
            }
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeViewGroup_doLayout(JNIEnv *env, jobject rawSelf,
                                                                                 bool changed, int left, int top,
                                                                                 int right, int bottom)
{
    bdn::platformEntryWrapper(
        [&]() {
            if (auto core =
                    bdn::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawSelf))) {
                core->startLayout();
            }
        },
        true, env);
}
