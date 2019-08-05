#include <bdn/AttributedString.h>

namespace bdn
{
    void AttributedString::fromJSON(const json &j)
    {
        if (j.count("html")) {
            fromHTML(j.at("html"));
        } else if (j.count("string")) {
            fromString(j.at("string"));
        }

        if (j.count("ranges")) {
            auto ranges = j.at("ranges");

            for (auto &range : ranges) {
                size_t start = 0;
                size_t length = -1;

                if (range.count("start")) {
                    start = range.at("start");
                }
                if (range.count("length")) {
                    length = range.at("length");
                }
                auto attributes = range.at("attributes");

                std::map<std::string, std::any> attributeMap;
                for (auto attribute : attributes.items()) {
                    attributeMap[attribute.key()] = attribute.value();
                }
                if (!attributeMap.empty()) {
                    addAttributes(attributeMap, {start, length});
                }
            }
        }
    }

    json AttributedString::toJSON() const { return {{"html", toHTML()}}; }
}
