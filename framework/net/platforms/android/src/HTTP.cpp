#include <bdn/config.h>
#include <bdn/HTTP.h>
#include <bdn/HTTPRequest.h>
#include <bdn/HTTPResponse.h>
#include <bdn/AppRunnerBase.h>

namespace bdn
{
    namespace net
    {
        namespace http
        {
            std::shared_ptr<HTTPResponse> request(HTTPRequest request) { return nullptr; }
        }
    }
}
