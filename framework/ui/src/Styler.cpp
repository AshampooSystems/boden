#include <bdn/log.h>
#include <bdn/ui/Styler.h>

namespace bdn::ui
{
    void Styler::setStyleSheet(std::shared_ptr<View> view, json json)
    {
        _data[view] = Data{json};
        rematchView(view);
    }

    void Styler::setCondition(std::string name, std::shared_ptr<Styler::condition> condition)
    {
        _conditions[name] = condition;
        rematchAllViews(name);
    }

    void Styler::rematchView(const std::shared_ptr<View> &view)
    {
        auto it = _data.find(view);
        if (it != _data.end()) {
            json result;

            if (!it->second.json.is_array()) {
                throw std::runtime_error("Root must be an array");
            }

            for (auto &option : it->second.json.items()) {
                if (option.value().count("if") == 0) {
                    result.merge_patch(option.value());
                } else {
                    auto &v_if = option.value().at("if");

                    bool doesMatch = true;

                    for (auto &matches : v_if.items()) {
                        auto matcher = _conditions.find(matches.key());

                        if (matcher == _conditions.end()) {
                            throw std::runtime_error("Invalid matcher specified");
                        }

                        it->second.usedMatchers.insert(matches.key());

                        if (!(*matcher->second)(matches.value())) {
                            doesMatch = false;
                        }
                    }

                    if (doesMatch) {
                        result.merge_patch(option.value());
                    }
                }
            }

            view->stylesheet = result;
        }
    }

    void Styler::rematchAllViews(const std::string &matcherName)
    {
        for (auto it = _data.begin(); it != _data.end();) {
            if (it->second.usedMatchers.find(matcherName) != it->second.usedMatchers.end()) {
                if (auto view = it->first.lock()) {
                    rematchView(view);
                } else {
                    it = _data.erase(it);
                    continue;
                }
            }

            ++it;
        }
    }
}
