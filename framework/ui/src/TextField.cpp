
#include <bdn/TextField.h>

namespace bdn
{

    TextField::TextField()
    {
        _onSubmit = std::make_shared<SimpleNotifier<const SubmitEvent &>>();

        text.onChange() += View::CorePropertyUpdater<String, ITextFieldCore>{this, &ITextFieldCore::setText};
    }

    void TextField::submit()
    {
        bdn::SubmitEvent event(shared_from_this());
        onSubmit().notify(event);
    }

    ISyncNotifier<const SubmitEvent &> &TextField::onSubmit() { return *_onSubmit; }
}
