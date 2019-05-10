#pragma once

#include <bdn/net/HTTPRequest.h>

namespace bdn::net
{
    class HTTPResponse
    {
      public:
        HTTPRequest originalRequest;

        String url;
        String header;
        String data;

        int responseCode{};
    };
}
