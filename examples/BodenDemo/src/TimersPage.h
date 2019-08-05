#pragma once

#include <bdn/foundation.h>
#include <bdn/ui.h>

#include "Page.h"

using namespace std::string_literals;
using namespace std::chrono_literals;

namespace bdn
{
    class TimersPage : public CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;
        void init() override;

      public:
        Property<Timer::Duration> interval = 10ms;

        Property<std::string> _stringToDurationProperty{TransformBacking<std::string, Timer::Duration>{
            interval, &TimersPage::durationToString,
            std::bind(&TimersPage::stringToDuration, this, std::placeholders::_1)}};

        static std::string durationToString(Timer::Duration duration)
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

        Timer::Duration stringToDuration(std::string s)
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

      private:
        void timerCallback();

      private:
        Timer _timer;
        StopWatch _watch;
        std::shared_ptr<ContainerView> _demoContainer;
        std::shared_ptr<Button> _demoBtn;
    };
}
