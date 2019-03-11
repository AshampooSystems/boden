
#include <bdn/ScrollView.h>

#include <bdn/AppRunnerBase.h>
#include <bdn/ScrollViewCore.h>

namespace bdn
{

    ScrollView::ScrollView(std::shared_ptr<UIProvider> uiProvider)
        : View(std::move(uiProvider)), verticalScrollingEnabled(true)
    {
        content.onChange() += [=](auto va) { _content.update(shared_from_this(), va->get()); };
    }

    void ScrollView::bindViewCore()
    {
        View::bindViewCore();

        auto scrollCore = core<ScrollViewCore>();
        scrollCore->content.bind(content);
        scrollCore->horizontalScrollingEnabled.bind(horizontalScrollingEnabled);
        scrollCore->verticalScrollingEnabled.bind(verticalScrollingEnabled);
    }

    void ScrollView::scrollClientRectToVisible(const Rect &area)
    {
        AppRunnerBase::assertInMainThread();

        auto scrollCore = core<ScrollViewCore>();
        scrollCore->scrollClientRectToVisible(area);
    }

    std::list<std::shared_ptr<View>> ScrollView::childViews()
    {
        AppRunnerBase::assertInMainThread();

        if (content.get() != nullptr) {
            return {content.get()};
        }

        return {};
    }

    void ScrollView::childViewStolen(std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        if (childView == content.get())
            content = nullptr;
    }
}
