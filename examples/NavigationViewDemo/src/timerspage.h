#pragma once

#include <bdn/foundation.h>
#include <bdn/ui.h>

#include "../../uidemo/src/uidemopage.h"

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
            nlohmann::json jsonStylesheet = {{"direction", FlexStylesheet::Direction::Column},
                                             {"flexGrow", 1.0},
                                             {"flexShrink", 1.0},
                                             {"alignItems", FlexStylesheet::Align::Stretch},
                                             {"padding", {{"all", 20.0}}},
                                             {"margin", {{"all", 2.0}}}};

            FlexStylesheet styleSheet = FlexJson(jsonStylesheet);
            setLayoutStylesheet(styleSheet);

            auto runningCtrl = std::make_shared<Switch>();
            runningCtrl->on = true;
            addChildView(makeRow("Running", runningCtrl));

            auto repeatCtrl = std::make_shared<Switch>();
            repeatCtrl->on = true;
            addChildView(makeRow("Repeat", repeatCtrl));

            auto intervalField = std::make_shared<TextField>();
            intervalField->text.bind(_stringToDurationProperty);
            intervalField->setLayoutStylesheet((FlexStylesheet)FlexMinimumSizeWidth(100));
            addChildView(makeRow("Interval", intervalField));

            _demoContainer = std::make_shared<ContainerView>();
            _demoContainer->setLayoutStylesheet(FlexJson({{"flexGrow", 1.0}}));

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

            /*            auto stringField = std::make_shared<TextField>();
                            stringField->text.bind(_stringProperty);
                            stringField->setLayoutStylesheet((FlexStylesheet)FlexMinimumSizeWidth(250));
                            addChildView(makeRow("String", stringField));

                            auto intField = std::make_shared<TextField>();
                            intField->text.bind(_stringToIntProperty);
                            intField->setLayoutStylesheet((FlexStylesheet)FlexMinimumSizeWidth(250));
                            addChildView(makeRow("Int", intField));

                            auto btnContainer = std::make_shared<ContainerView>();
                            btnContainer->setLayoutStylesheet(FlexJson({{"direction", FlexStylesheet::Direction::Row},
                                                                        {"flexGrow", 0.0},
                                                                        {"flexShrink", 0.0},
                                                                        {"justifyContent",
                   FlexStylesheet::Align::FlexEnd}}));

                            auto plusBtn = std::make_shared<Button>();
                            plusBtn->label = "+";
                            auto minusBtn = std::make_shared<Button>();
                            minusBtn->label = "-";

                            plusBtn->onClick() += [=](auto) { ++_intProperty; };
                            minusBtn->onClick() += [=](auto) { --_intProperty; };

                            btnContainer->addChildView(plusBtn);
                            btnContainer->addChildView(minusBtn);

                            addChildView(btnContainer);

                            auto header = std::make_shared<Label>();
                            header->text = "Streaming property:";
                            header->setLayoutStylesheet(FlexMarginBottom(5) << FlexShrink(0));
                            addChildView(header);

                            auto streamLabel = std::make_shared<Label>();
                            streamLabel->text.bind(_streamProperty);
                            addChildView(streamLabel);
                */
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
