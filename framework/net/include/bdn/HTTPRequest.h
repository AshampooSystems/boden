#pragma once

#include <bdn/HTTP.h>
#include <bdn/String.h>
#include <bdn/property/Property.h>

namespace bdn
{
    namespace net
    {
        class HTTPResponse;

        /*!
         * \brief The HTTPRequest class
         *
         * \example
         *         auto response = net::request({"http://www.google.de", [](net::HTTPRequest::Status s) { std::cout <<
         * "Done and status is:" << s << std::endl; }});
         */
        class HTTPRequest
        {
          public:
            enum class Status
            {
                InProgress,
                Done
            };

            using DoneHandler = std::function<void(std::shared_ptr<HTTPResponse>)>;

          public:
            //! The http method
            http::Method method;
            //! The http url
            String url;
            //! Additional header fields for PUT requests
            String header;

            DoneHandler doneHandler;

          public:
            HTTPRequest() = default;
            HTTPRequest(String requestUrl, DoneHandler requestDoneHandler)
                : url(requestUrl), doneHandler(requestDoneHandler)
            {}
            HTTPRequest(http::Method requestMethod, String requestUrl, DoneHandler requestDoneHandler)
                : method(requestMethod), url(requestUrl), doneHandler(requestDoneHandler)
            {}
        };
    }
}
