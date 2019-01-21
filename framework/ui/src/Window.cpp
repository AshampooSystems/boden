
#include <bdn/Window.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/debug.h>
#include <bdn/UiAppControllerBase.h>

namespace bdn
{

    Window::Window(std::shared_ptr<IUiProvider> uiProvider)
    {
        title.onChange() += CorePropertyUpdater<String, IWindowCore>(this, &IWindowCore::setTitle);

        visible = false;

        visible.onChange() += [this](auto va) {
            if (va->get()) {
                this->_initCore();
            }
        };

        _uiProvider = (uiProvider != nullptr) ? uiProvider : UiAppControllerBase::get()->getUiProvider();
    }

    void Window::_initCore()
    {
        // initCore might throw an exception in some cases (for example if the
        // view type is not supported). we want to propagate that exception
        // upwards.

        // If the core is not null then we already have a core. We do nothing in
        // that case.
        if (_core == nullptr) {
            _uiProvider = determineUiProvider();

            if (_uiProvider != nullptr)
                _core = _uiProvider->createViewCore(getCoreTypeName(), shared_from_this());

            if (_contentView) {
                _contentView->_initCore();
            }
            // our old sizing info is obsolete when the core has changed.
            invalidateSizingInfo(View::InvalidateReason::standardPropertyChanged);
        }
    }

    void Window::_deinitCore()
    {
        View::_deinitCore();

        if (_contentView) {
            _contentView->_deinitCore();
        }
    }

    void Window::setContentView(std::shared_ptr<View> contentView)
    {
        AppRunnerBase::assertInMainThread();

        reinitCore();

        if (contentView != _contentView) {
            if (_contentView != nullptr)
                _contentView->_setParentView(nullptr);

            _contentView = contentView;

            if (_contentView != nullptr)
                _contentView->_setParentView(shared_from_this());

            invalidateSizingInfo(InvalidateReason::childAddedOrRemoved);
        }
    }

    std::shared_ptr<View> Window::getContentView()
    {
        AppRunnerBase::assertInMainThread();

        return _contentView;
    }

    std::shared_ptr<const View> Window::getContentView() const
    {
        AppRunnerBase::assertInMainThread();

        return _contentView;
    }

    void Window::requestAutoSize()
    {
        std::shared_ptr<IWindowCore> core = std::dynamic_pointer_cast<IWindowCore>(getViewCore());

        // forward the request to the core. Depending on the platform
        // it may be that the UI uses a layout coordinator provided by the
        // system, rather than our own.
        if (core != nullptr)
            core->requestAutoSize();
    }

    void Window::requestCenter()
    {
        // Since autosizing is asynchronous, this must also be done via
        // the layout coordination system. Otherwise we might center with the
        // old size and would then autoSize afterwards. So, also forward this to
        // the core.

        std::shared_ptr<IWindowCore> core = std::dynamic_pointer_cast<IWindowCore>(getViewCore());

        if (core != nullptr)
            core->requestCenter();
    }

    std::list<std::shared_ptr<View>> Window::getChildViews() const
    {
        AppRunnerBase::assertInMainThread();
        if (_contentView) {
            return {_contentView};
        }
        return {};
    }

    void Window::removeAllChildViews() { setContentView(nullptr); }

    std::shared_ptr<View> Window::findPreviousChildView(std::shared_ptr<View> childView)
    {
        // we do not have multiple child views with an order - just a single
        // content view
        return nullptr;
    }

    void Window::_childViewStolen(std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        if (childView == _contentView)
            _contentView = nullptr;
    }

    std::shared_ptr<IUiProvider> Window::determineUiProvider(std::shared_ptr<View> parentView)
    {
        // our Ui provider never changes. Just return the current one.
        return std::dynamic_pointer_cast<IUiProvider>(_uiProvider);
    }
}
