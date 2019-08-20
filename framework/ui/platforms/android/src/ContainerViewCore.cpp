
#include <bdn/android/ContainerViewCore.h>
#include <bdn/entry.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(ContainerView, bdn::ui::android::ContainerViewCore, ContainerView)
}

namespace bdn::ui::android
{
    ContainerViewCore::ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<bdn::android::wrapper::NativeViewGroup>(viewCoreFactory))
    {}

    ContainerViewCore::~ContainerViewCore() = default;

    double ContainerViewCore::getUIScaleFactor() const { return ViewCore::getUIScaleFactor(); }

    void ContainerViewCore::addChildView(std::shared_ptr<View> child)
    {
        if (auto childCore = child->core<android::ViewCore>()) {
            _children.push_back(child);
            auto group = getJViewAS<bdn::android::wrapper::NativeViewGroup>();
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
            auto it = std::remove(_children.begin(), _children.end(), child);

            if (it != _children.end()) {
                _children.erase(it, _children.end());
                auto group = getJViewAS<bdn::android::wrapper::NativeViewGroup>();
                group.removeView(childCore->getJView());
            }

        } else {
            throw std::runtime_error("Tried removing Child with incompatible core");
        }

        scheduleLayout();
    }

    std::vector<std::shared_ptr<View>> ContainerViewCore::childViews() const { return _children; }

    void ContainerViewCore::visitInternalChildren(const std::function<void(std::shared_ptr<View::Core>)> &function)
    {
        for (auto &child : _children) {
            if (auto childCore = child->core<View::Core>()) {
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
                    bdn::ui::android::viewCoreFromJavaViewRef(bdn::java::Reference::convertExternalLocal(rawSelf))) {
                core->startLayout();
            }
        },
        true, env);
}
