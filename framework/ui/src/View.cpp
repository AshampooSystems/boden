#include <bdn/ui/View.h>

#include <bdn/ui/ViewCoreFactory.h>

#include <bdn/ui/UIApplicationController.h>

#include <utility>

namespace bdn::ui
{
    bool &View::debugViewEnabled()
    {
        static bool s_debugViewEnabled = false;
        return s_debugViewEnabled;
    }

    View::View(std::shared_ptr<ViewCoreFactory> viewCoreFactory)
        : _viewCoreFactory(viewCoreFactory ? std::move(viewCoreFactory) : UIApplicationController::topViewCoreFactory())
    {
        if (!_viewCoreFactory) {
            throw std::runtime_error("Couldn't get ViewCore Factory!");
        }

        stylesheet.onChange() += [=](auto &property) {
            updateFromStylesheet();
            if (auto layout = _layout.get()) {
                layout->updateStylesheet(this);
            }
        };

        isLayoutRoot.onChange() += [=](auto) { updateLayout(_layout.get(), _layout.get()); };

        visible.onChange() += [=](auto) {
            if (auto layout = _layout.get()) {
                layout->updateStylesheet(this);
            }
        };

        registerCoreCreatingProperties(this, &visible, &geometry, &stylesheet);
    }

    View::~View()
    {
        if (auto layout = _layout.get()) {
            layout->unregisterView(this);
        }
    }

    void View::setLayout(std::shared_ptr<Layout> layout)
    {
        auto oldLayout = _layout.set(std::move(layout));
        updateLayout(oldLayout, _layout.get());
    }

    void View::offerLayout(std::shared_ptr<Layout> layout)
    {
        auto oldLayout = _layout.setOffered(std::move(layout));

        updateLayout(oldLayout, _layout.get());
    }

    void View::updateLayout(const std::shared_ptr<Layout> &oldLayout, const std::shared_ptr<Layout> &newLayout)
    {
        if (oldLayout) {
            oldLayout->unregisterView(this);
        }

        if (newLayout) {
            newLayout->registerView(this);
        }

        if (oldLayout != newLayout) {
            viewCore()->setLayout(newLayout);
        }

        for (const auto &child : childViews()) {
            child->offerLayout(newLayout);
        }
    }

    std::shared_ptr<Layout> View::getLayout() { return _layout.get(); }

    void View::setParentView(const std::shared_ptr<View> &parentView)
    {
        if (!canMoveToParentView(parentView)) {
            throw std::runtime_error("Cannot move to parent View!");
        }

        if (auto oldParent = _parentView.lock()) {
            oldParent->childViewStolen(shared_from_this());
        }

        _parentView = parentView;

        if (parentView) {
            offerLayout(parentView->getLayout());
        } else {
            offerLayout(nullptr);
        }
    }

    void View::updateFromStylesheet()
    {
        if (stylesheet->count("visible")) {
            visible = (bool)stylesheet->at("visible");
        } else {
            visible = true;
        }

        if (auto core = viewCore()) {
            core->updateFromStylesheet(stylesheet.get());
        }
    }

    bool View::canMoveToParentView(const std::shared_ptr<View> &parentView)
    {
        if (!parentView) {
            return true;
        }

        return _viewCoreFactory == parentView->viewCoreFactory() && viewCore()->canMoveToParentView(parentView);
    }

    void View::scheduleLayout()
    {
        if (auto core = viewCore()) {
            core->scheduleLayout();
        } else {
            _hasLayoutSchedulePending = true;
        }
    }

    const std::type_info &View::typeInfoForCoreCreation() const { return typeid(*this); }

    void View::lazyInitCore() const
    {
        if (_core) {
            return;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        auto un_const_this = const_cast<View *>(this);

        _core = _viewCoreFactory->createViewCore(typeInfoForCoreCreation());
        un_const_this->bindViewCore();
    }

    std::shared_ptr<View::Core> View::viewCore()
    {
        lazyInitCore();
        return _core;
    }

    std::shared_ptr<View::Core> View::viewCore() const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        return const_cast<View *>(this)->viewCore();
    }

    void View::bindViewCore()
    {
        viewCore()->visible.bind(visible);
        viewCore()->geometry.bind(geometry);

        _layoutCallbackReceiver = viewCore()->_layoutCallback.set([=]() { onCoreLayout(); });
        _dirtyCallbackReceiver = viewCore()->_dirtyCallback.set([=]() { onCoreDirty(); });
    }

    void View::onCoreLayout()
    {
        if (auto layout = getLayout()) {
            layout->layout(this);
        }
    }

    void View::onCoreDirty()
    {
        if (auto layout = getLayout()) {
            layout->markDirty(this);
        }
    }

    Size View::sizeForSpace(Size availableSpace) const { return viewCore()->sizeForSpace(availableSpace); }

    View::Core::Core(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : _viewCoreFactory(std::move(viewCoreFactory)) {}
}
