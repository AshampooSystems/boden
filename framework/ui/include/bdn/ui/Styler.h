#pragma once

#include <bdn/Json.h>
#include <bdn/ui/View.h>
#include <map>
#include <memory>
#include <set>

namespace bdn::ui
{
    class Styler
    {
        struct Data // TODO: Find better name
        {
            json json;
            std::set<std::string> usedMatchers;
        };

      public:
        struct condition
        {
            virtual ~condition() = default;
            virtual bool operator()(const json::value_type &) = 0;
        };

        struct equals_condition : public condition
        {
            equals_condition(json::value_type targetValue) : _targetValue(targetValue) {}
            bool operator()(const json::value_type &value) override { return value == _targetValue; }

            json::value_type _targetValue;
        };

        struct less_condition : public condition
        {
            less_condition(json::value_type targetValue) : _targetValue(targetValue) {}
            bool operator()(const json::value_type &value) override { return value < _targetValue; }

            json::value_type _targetValue;
        };
        struct greater_equal_condition : public condition
        {
            greater_equal_condition(json::value_type targetValue) : _targetValue(targetValue) {}
            bool operator()(const json::value_type &value) override { return value >= _targetValue; }

            json::value_type _targetValue;
        };

      public:
        void setStyleSheet(std::shared_ptr<View> view, json stylesheet);
        void setCondition(std::string name, std::shared_ptr<condition> condition);

      private:
        void rematchView(const std::shared_ptr<View> &view);
        void rematchAllViews(const std::string &matcherName);

      private:
        std::map<std::weak_ptr<View>, Data, std::owner_less<std::weak_ptr<View>>> _data;
        std::map<std::string, std::shared_ptr<condition>> _conditions;
    };
}
