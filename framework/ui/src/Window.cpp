#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/debug.h>
#include <bdn/UiAppControllerBase.h>

namespace bdn
{

    Window::Window(IUiProvider *uiProvider)
    {
        // windows are invisible by default
        setVisible(false);

        _uiProvider = (uiProvider != nullptr) ? uiProvider : UiAppControllerBase::get()->getUiProvider().getPtr();

        reinitCore();
    }

    void Window::setContentView(View *contentView)
    {
        Thread::assertInMainThread();

        if (contentView != _contentView) {
            if (_contentView != nullptr)
                _contentView->_setParentView(nullptr);

            _contentView = contentView;

            if (_contentView != nullptr)
                _contentView->_setParentView(this);

            invalidateSizingInfo(InvalidateReason::childAddedOrRemoved);
        }
    }

    void Window::requestAutoSize()
    {
        P<IWindowCore> core = cast<IWindowCore>(getViewCore());

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

        P<IWindowCore> core = cast<IWindowCore>(getViewCore());

        if (core != nullptr)
            core->requestCenter();
    }
}
