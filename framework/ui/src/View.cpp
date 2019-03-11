#include <bdn/View.h>

#include <bdn/ProgrammingError.h>
#include <bdn/UIProvider.h>
#include <bdn/debug.h>

#include <bdn/UIAppControllerBase.h>

namespace bdn
{

    View::View(std::shared_ptr<UIProvider> uiProvider)
        : _uiProvider(uiProvider ? std::move(uiProvider) : UIAppControllerBase::get()->uiProvider()),
          _hasLayoutSchedulePending(false)
    {
        if (!_uiProvider) {
            throw std::runtime_error("Couldn't get UI Provider!");
        }

        visible = true; // most views are initially visible

        layoutStylesheet.onChange() += [=](auto va) {
            if (auto layout = _layout.get()) {
                layout->updateStylesheet(this);
            }
        };

        registerCoreCreatingProperties(this, &visible, &geometry, &layoutStylesheet);
    }

    View::~View() { setLayout(nullptr); }

    void View::setLayout(std::shared_ptr<Layout> layout)
    {
        auto oldLayout = _layout.set(layout);
        updateLayout(oldLayout, _layout.get());
    }

    void View::offerLayout(std::shared_ptr<Layout> layout)
    {
        auto oldLayout = _layout.setOffered(std::move(layout));

        updateLayout(oldLayout, _layout.get());
    }

    void View::updateLayout(std::shared_ptr<Layout> oldLayout, std::shared_ptr<Layout> newLayout)
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

        for (auto child : childViews()) {
            child->offerLayout(newLayout);
        }
    }

    std::shared_ptr<Layout> View::getLayout() { return _layout.get(); }

    void View::setParentView(std::shared_ptr<View> parentView)
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

    bool View::canMoveToParentView(std::shared_ptr<View> parentView)
    {
        if (!parentView)
            return true;

        return _uiProvider == parentView->uiProvider() && viewCore()->canMoveToParentView(parentView);
    }

    void View::scheduleLayout()
    {
        if (auto core = viewCore()) {
            core->scheduleLayout();
        } else {
            _hasLayoutSchedulePending = true;
        }
    }

    void View::lazyInitCore() const
    {
        if (_core)
            return;

        auto un_const_this = const_cast<View *>(this);

        _core = _uiProvider->createViewCore(viewCoreTypeName());
        un_const_this->bindViewCore();
    }

    std::shared_ptr<ViewCore> View::viewCore()
    {
        lazyInitCore();
        return _core;
    }

    std::shared_ptr<ViewCore> View::viewCore() const { return const_cast<View *>(this)->viewCore(); }

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
}
