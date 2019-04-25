#pragma once

#include <bdn/foundation.h>
#include <bdn/ui.h>

#include "uidemopage.h"

namespace bdn
{

    class Timers : public ContainerView
    {
      public:
        Property<Timer::Duration> interval = 10ms;

        Property<String> _stringToDurationProperty{Transform<String, Timer::Duration>{
            interval, &Timers::durationToString, std::bind(&Timers::stringToDuration, this, std::placeholders::_1)}};

        static String durationToString(Timer::Duration duration)
        {
            std::ostringstream stream;
            if (duration < 1000ns) {
                auto v = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
                stream << v << "ns";
            } else if (duration < 1000us) {
                auto v = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
                stream << v << "us";
            } else if (duration < 1000ms) {
                auto v = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                stream << v << "ms";
            } else {
                stream << duration.count();
                stream << "s";
            }

            return stream.str();
        }

        Timer::Duration stringToDuration(String s)
        {
            std::istringstream stream(s);
            float v;
            stream >> v;
            if (stream.good()) {
                if (cpp20::ends_with(s, "ns")) {
                    return std::chrono::nanoseconds((long long)v);
                }
                if (cpp20::ends_with(s, "us")) {
                    return std::chrono::microseconds((long long)v);
                }
                if (cpp20::ends_with(s, "ms")) {
                    return std::chrono::milliseconds((long long)v);
                }
                if (cpp20::ends_with(s, "s")) {
                    return std::chrono::duration<float>(v);
                }
            }

            return interval.get();
        }

      public:
        Timers() {}

        void timerCallback()
        {
            auto btnSize = _demoBtn->geometry->size();
            auto containerSize = _demoContainer->geometry->size();
            auto timeInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(_watch.elapsed());

            auto x = ((std::sin(timeInSeconds.count() * 0.5) + 1.0) / 2.0f) * (containerSize.width - btnSize.width);
            auto y = ((std::cos(timeInSeconds.count() * 0.5) + 1.0) / 2.0f) * (containerSize.height - btnSize.height);
            _demoBtn->geometry = {{x, y}, btnSize};
        }

        void init()
        {
            stylesheet = FlexJsonStringify(
                {"direction" : "Column", "flexGrow" : 1.0, "flexShrink" : 1.0, "alignItems" : "Stretch"});

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

      private:
        Timer _timer;
        StopWatch _watch;
        std::shared_ptr<ContainerView> _demoContainer;
        std::shared_ptr<Button> _demoBtn;

      public:
        const std::type_info &typeInfoForCoreCreation() const override { return typeid(ContainerView); }
    };

    auto createTimerDemoPage()
    {
        auto page = std::make_shared<Timers>();
        page->init();
        return page;
    }
}
