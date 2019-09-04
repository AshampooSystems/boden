#pragma once

#include <bdn/net/HTTP.h>
#include <bdn/property/Property.h>
#include <string>

#include <utility>

#include <utility>

namespace bdn::net
{
    class HTTPResponse;

    class HTTPRequest
    {
      public:
        using DoneHandler = std::function<void(std::shared_ptr<HTTPResponse>)>;

      public:
        http::Method method{bdn::net::http::Method::GET};
        std::string url;
        std::string header;

        DoneHandler doneHandler;

      public:
        HTTPRequest() = default;
        HTTPRequest(std::string requestUrl, DoneHandler requestDoneHandler);
        HTTPRequest(http::Method requestMethod, std::string requestUrl, DoneHandler requestDoneHandler);
    };
}
