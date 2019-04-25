#pragma once

#include <bdn/String.h>
#include <nlohmann/json.hpp>

namespace bdn
{
    using json = nlohmann::json;
}

#define JsonStringify(str...) bdn::json::parse(#str)
