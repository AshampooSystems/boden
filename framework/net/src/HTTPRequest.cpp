#include <bdn/net/HTTPRequest.h>

namespace bdn::net
{
    HTTPRequest::HTTPRequest(std::string requestUrl, DoneHandler requestDoneHandler)
        : url(std::move(std::move(requestUrl))), doneHandler(std::move(std::move(requestDoneHandler)))
    {}

    HTTPRequest::HTTPRequest(http::Method requestMethod, std::string requestUrl, DoneHandler requestDoneHandler)
        : method(requestMethod), url(std::move(std::move(requestUrl))),
          doneHandler(std::move(std::move(requestDoneHandler)))
    {}
}
