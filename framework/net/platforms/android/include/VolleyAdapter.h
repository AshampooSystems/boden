#pragma once

#include <bdn/java/JObject.h>
#include <bdn/java/JString.h>

#include <bdn/HTTP.h>

#include <functional>

namespace bdn
{
    namespace net
    {
        namespace http
        {
            namespace android
            {
                constexpr char kVolleyAdapterClassName[] = "io/boden/java/VolleyAdapter";

                class JVolleyAdapter : public java::JTObject<kVolleyAdapterClassName>
                {
                  public:
                    using JTObject<kVolleyAdapterClassName>::JTObject;

                    java::Method<void(int, String, std::shared_ptr<bdn::net::HTTPResponse>)> request{this, "request"};

                    static constexpr int toVolleyRequestMethod(bdn::net::http::Method bdnHttpMethod)
                    {
                        switch (bdnHttpMethod) {
                        case bdn::net::http::Method::GET:
                            return 0;
                        case bdn::net::http::Method::POST:
                            return 1;
                        case bdn::net::http::Method::PUT:
                            return 2;
                        case bdn::net::http::Method::DELETE:
                            return 3;
                        case bdn::net::http::Method::HEAD:
                            return 4;
                        case bdn::net::http::Method::OPTIONS:
                            return 5;
                        case bdn::net::http::Method::TRACE:
                            return 6;
                        case bdn::net::http::Method::CONNECT:
                            return -1;
                        }
                        return -1;
                    }
                };
            }
        }
    }
}
