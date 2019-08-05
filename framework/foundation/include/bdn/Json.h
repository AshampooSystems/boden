#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace bdn
{
    using json = nlohmann::json;
}

#define JsonStringify(str...) bdn::json::parse(#str)
