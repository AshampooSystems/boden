#pragma once

#include <bdn/HTTPRequest.h>
#include <bdn/Base.h>

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

            int responseCode;
        };
    }
}
