#pragma once

#include <bdn/HTTPRequest.h>

namespace bdn
{
    namespace net
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
}
