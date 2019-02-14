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

        needLayout(InvalidateReason::childAddedOrRemoved);
    }

    void Stack::popView()
    {
        _stack.pop_back();

        if (auto core = std::dynamic_pointer_cast<StackCore>(getViewCore())) {
            core->popView();
        }
        needLayout(InvalidateReason::childAddedOrRemoved);
    }

    void Stack::_initCore()
    {
        View::_initCore();

        if (auto core = std::dynamic_pointer_cast<StackCore>(getViewCore())) {
            for (auto page : _stack) {
                core->pushView(page.view, page.title);
            }
        }
    }

    void Stack::_deinitCore() { View::_deinitCore(); }
}
