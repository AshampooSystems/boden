#include <bdn/ViewCoreFactory.h>

namespace bdn
{
    std::shared_ptr<ViewCore> ViewCoreFactory::createViewCore(const String &coreTypeName)
    {
        auto core = create(coreTypeName, shared_from_this());

        if (!core) {
            throw ViewCoreTypeNotSupportedError(coreTypeName);
        }

        return *core;
    }

    ViewCoreFactory::ContextStack *ViewCoreFactory::contextStack()
    {
        static ContextStack s_contextStack;
        return &s_contextStack;
    }

    void ViewCoreFactory::pushContext(std::shared_ptr<UIContext> &context)
    {
        auto stack = contextStack();
        stack->push_back(context);
    }

    void ViewCoreFactory::popContext()
    {
        auto stack = contextStack();
        stack->pop_back();
    }
}
