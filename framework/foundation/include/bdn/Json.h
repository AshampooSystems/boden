#pragma once

#include <bdn/property/Property.h>
#include <nlohmann/json.hpp>
#include <string>

namespace bdn
{
    using json = nlohmann::json;

    inline void mergeProperty(Property<json> &property, const json &rhs)
    {
        auto data = property.get();
        data.merge_patch(rhs);
        property = data;
    }

    inline Property<json> &operator|=(Property<json> &prop, const json &j)
    {
        mergeProperty(prop, j);
        return prop;
    }
}

#define JsonStringify(str...) bdn::json::parse(#str)
