
#include <bdn/Checkbox.h>

namespace bdn
{

    Checkbox::Checkbox()
    {
        _onClick = std::make_shared<SimpleNotifier<const ClickEvent &>>();

        label.onChange() += View::CorePropertyUpdater<String, CheckboxCore>{
            this, &CheckboxCore::setLabel, [](auto &inf) { inf.influencesPreferredSize(); }};

        state.onChange() += View::CorePropertyUpdater<TriState, CheckboxCore>{this, &CheckboxCore::setState};
    }

    ISyncNotifier<const ClickEvent &> &Checkbox::onClick() { return *_onClick; }
}
