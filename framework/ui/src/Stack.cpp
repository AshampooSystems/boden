#include <bdn/Stack.h>
#include <bdn/StackCore.h>

#include <utility>

namespace bdn
{

    Stack::Stack(std::shared_ptr<UIProvider> uiProvider) : View(std::move(uiProvider)) {}

    Stack::~Stack() = default;

    void Stack::pushView(std::shared_ptr<View> view, String title)
    {
        if (auto core = std::dynamic_pointer_cast<StackCore>(viewCore())) {
            core->pushView(std::move(view), std::move(title));
        }
    }

    void Stack::popView()
    {
        if (auto stackCore = core<StackCore>()) {
            stackCore->popView();
        }
    }

    std::list<std::shared_ptr<View>> Stack::childViews()
    {
        if (auto stackCore = core<StackCore>()) {
            return stackCore->childViews();
        }
        return {};
    }

    void Stack::bindViewCore() { View::bindViewCore(); }
}
