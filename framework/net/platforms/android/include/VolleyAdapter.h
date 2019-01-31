#pragma once

#include <bdn/java/JObject.h>
#include <bdn/java/JNativeStrongPointer.h>
#include <bdn/java/JString.h>

#include <bdn/HTTP.h>

namespace bdn
{
    namespace net
    {
        namespace http
        {
            namespace android
            {

                constexpr char kVolleyAdapterName[] = "io/boden/java/VolleyAdapter";

                class JVolleyAdapter : public bdn::java::JTObject<kVolleyAdapterName>
                {
                  public:
                    using JTObject::JTObject;

                    static constexpr char requestMethodName[] = "request";

                    void request(bdn::net::http::Method requestMethod, String url,
                                 std::shared_ptr<bdn::net::HTTPResponse> response)
                    {
                        int volleyRequestMethod = toVolleyRequestMethod(requestMethod);
                        bdn::java::JNativeStrongPointer responsePtr(std::static_pointer_cast<Base>(response));
                        invoke<void, requestMethodName>(volleyRequestMethod, bdn::java::JString(url), responsePtr);
                    }

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
