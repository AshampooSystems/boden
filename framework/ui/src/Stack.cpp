#include <bdn/Stack.h>
#include <bdn/StackCore.h>

namespace bdn
{

    Stack::Stack() {}

    Stack::~Stack() {}

    void Stack::pushView(std::shared_ptr<View> view, String title)
    {
        _stack.push_back(StackEntry{view, title});

        if (auto core = std::dynamic_pointer_cast<StackCore>(getViewCore())) {
            core->pushView(view, title);
        }
    }

    void Stack::popView()
    {
        _stack.pop_back();

        if (auto core = std::dynamic_pointer_cast<StackCore>(getViewCore())) {
            core->popView();
        }
    }

    void Stack::_initCore(std::shared_ptr<UIProvider> uiProvider)
    {
        View::_initCore(uiProvider);

        if (auto core = std::dynamic_pointer_cast<StackCore>(getViewCore())) {
            for (auto page : _stack) {
                core->pushView(page.view, page.title);
            }
        }
    }

    void Stack::_deinitCore() { View::_deinitCore(); }

    std::list<std::shared_ptr<View>> Stack::getChildViews() const
    {
        if (auto core = std::dynamic_pointer_cast<StackCore>(getViewCore())) {
            return core->getChildViews();
        }
        return {};
    }
}
