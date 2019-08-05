#pragma once

#include <bdn/net/HTTPRequest.h>

namespace bdn::net
{
    class HTTPResponse
    {
      public:
        HTTPRequest originalRequest;

        std::string url;
        std::string header;
        std::string data;

        int responseCode{};
    };
}
