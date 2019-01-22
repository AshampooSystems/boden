#pragma once

#include <memory>

namespace bdn
{
    namespace net
    {

        class HTTPRequest;
        class HTTPResponse;

        namespace http
        {
            enum class Method
            {
                GET,
                POST,
                PUT,
                DELETE,
                HEAD,
                TRACE,
                OPTIONS,
                CONNECT
            };
        }

        namespace http
        {
            std::shared_ptr<HTTPResponse> request(HTTPRequest request);
        }
    }
}
