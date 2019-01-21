
#include <bdn/Checkbox.h>

namespace bdn
{

    Checkbox::Checkbox()
    {
        _onClick = std::make_shared<SimpleNotifier<const ClickEvent &>>();

        label.onChange() += View::CorePropertyUpdater<String, IToggleCoreBase>{
            this, &IToggleCoreBase::setLabel, [](auto &inf) { inf.influencesPreferredSize(); }};

        state.onChange() += View::CorePropertyUpdater<TriState, ICheckboxCore>{this, &ICheckboxCore::setState};
    }

    ISyncNotifier<const ClickEvent &> &Checkbox::onClick() { return *_onClick; }
}
