
#include <bdn/Toggle.h>

namespace bdn
{

    Toggle::Toggle()
    {
        _onClick = std::make_shared<SimpleNotifier<const ClickEvent &>>();
        label.onChange() += View::CorePropertyUpdater<String, IToggleCoreBase>{
            this, &IToggleCoreBase::setLabel, [](auto &inf) { inf.influencesPreferredSize(); }};

        on.onChange() += View::CorePropertyUpdater<bool, ISwitchCore>{this, &ISwitchCore::setOn};

        on.onChange() += [this](auto va) { state = on ? TriState::on : TriState::off; };
    }

    ISyncNotifier<const ClickEvent &> &Toggle::onClick() { return *_onClick; }
}
