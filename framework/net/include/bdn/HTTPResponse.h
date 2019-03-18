#pragma once

#include <bdn/Base.h>
#include <bdn/HTTPRequest.h>

namespace bdn
{
    namespace net
    {
        class HTTPResponse : public Base
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
