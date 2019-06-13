#include "TimersPage.h"
#include <bdn/ui/yoga.h>

using namespace std::string_literals;
using namespace std::chrono_literals;

namespace bdn
{

    void TimersPage::timerCallback()
    {
        auto btnSize = _demoBtn->geometry->size();
        auto containerSize = _demoContainer->geometry->size();
        auto timeInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(_watch.elapsed());

        auto x = ((std::sin(timeInSeconds.count() * 0.5) + 1.0) / 2.0f) * (containerSize.width - btnSize.width);
        auto y = ((std::cos(timeInSeconds.count() * 0.5) + 1.0) / 2.0f) * (containerSize.height - btnSize.height);
        _demoBtn->geometry = {{x, y}, btnSize};
    }

    void TimersPage::init()
    {
        stylesheet =
            FlexJsonStringify({"direction" : "Column", "flexGrow" : 1.0, "flexShrink" : 1.0, "alignItems" : "Stretch"});

        auto runningCtrl = std::make_shared<Switch>();
        runningCtrl->on = true;
        addChildView(makeRow("Running", runningCtrl));

        auto repeatCtrl = std::make_shared<Switch>();
        repeatCtrl->on = true;
        addChildView(makeRow("Repeat", repeatCtrl));

        auto intervalField = std::make_shared<TextField>();
        intervalField->text.bind(_stringToDurationProperty);
        intervalField->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 100.0}});
        addChildView(makeRow("Interval", intervalField));

        _demoContainer = std::make_shared<ContainerView>();
        _demoContainer->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        _demoBtn = std::make_shared<Button>();
        _demoBtn->setLayout(nullptr);
        _demoBtn->label = "Bouncy";
        _demoBtn->geometry = Rect{{0, 0}, _demoBtn->sizeForSpace()};

        _demoContainer->addChildView(_demoBtn);

        addChildView(_demoContainer);

        _timer.repeat = true;
        _timer.onTriggered() += [=]() { timerCallback(); };
        _timer.interval = 10ms;

        _timer.running.bind(runningCtrl->on);
        _timer.repeat.bind(repeatCtrl->on);
        _timer.interval.bind(interval);

        _watch.start();
    }
}
