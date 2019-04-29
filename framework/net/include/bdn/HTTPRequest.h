#pragma once

#include <bdn/HTTP.h>
#include <bdn/String.h>
#include <bdn/property/Property.h>

#include <utility>

#include <utility>

namespace bdn
{
    namespace net
    {
        class HTTPResponse;

        class HTTPRequest
        {
          public:
            using DoneHandler = std::function<void(std::shared_ptr<HTTPResponse>)>;

          public:
            http::Method method{bdn::net::http::Method::GET};
            String url;
            String header;

            DoneHandler doneHandler;

          public:
            HTTPRequest() = default;
            HTTPRequest(String requestUrl, DoneHandler requestDoneHandler)
                : url(std::move(std::move(requestUrl))), doneHandler(std::move(std::move(requestDoneHandler)))
            {}
            HTTPRequest(http::Method requestMethod, String requestUrl, DoneHandler requestDoneHandler)
                : method(requestMethod), url(std::move(std::move(requestUrl))),
                  doneHandler(std::move(std::move(requestDoneHandler)))
            {}
        };
    }
}
