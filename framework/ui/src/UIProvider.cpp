#include <bdn/UIProvider.h>

namespace bdn
{
    std::shared_ptr<ViewCore> UIProvider::createViewCore(const String &coreTypeName)
    {
        auto core = create(coreTypeName, shared_from_this());

        if (!core) {
            throw ViewCoreTypeNotSupportedError(coreTypeName);
        }

        return *core;
    }

    UIProvider::ContextStack *UIProvider::contextStack()
    {
        static ContextStack s_contextStack;
        return &s_contextStack;
    }

    void UIProvider::pushContext(std::shared_ptr<UIContext> &context)
    {
        auto stack = contextStack();
        stack->push_back(context);
    }

    void UIProvider::popContext()
    {
        auto stack = contextStack();
        stack->pop_back();
    }
}
