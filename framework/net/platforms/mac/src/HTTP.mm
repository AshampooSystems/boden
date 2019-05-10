#include <bdn/Application.h>
#include <bdn/config.h>
#include <bdn/net/HTTP.h>
#include <bdn/net/HTTPRequest.h>
#include <bdn/net/HTTPResponse.h>

#import <bdn/foundationkit/stringUtil.hh>

#import <Foundation/Foundation.h>

namespace bdn
{
    namespace net
    {
        namespace http
        {
            void request(HTTPRequest request)
            {
                std::shared_ptr<HTTPResponse> response = std::make_shared<HTTPResponse>();

                response->originalRequest = request;

                NSURLSession *session = [NSURLSession sharedSession];
                NSURL *nsURL = [NSURL URLWithString:fk::stringToNSString(request.url)];
                if (nsURL == nullptr) {
                    return;
                }

                NSURLSessionDataTask *dataTask = [session
                      dataTaskWithURL:nsURL
                    completionHandler:^(NSData *_Nullable nsData, NSURLResponse *_Nullable nsResponse,
                                        NSError *_Nullable error) {
                      App()->dispatchQueue()->dispatchAsync([nsData, nsResponse, response]() {
                          auto nsHTTPResponse = (NSHTTPURLResponse *)nsResponse;

                          response->url = fk::nsStringToString([nsHTTPResponse.URL absoluteString]);
                          response->responseCode = (int)nsHTTPResponse.statusCode;

                          response->header =
                              fk::nsStringToString([NSString stringWithFormat:@"%@", nsHTTPResponse.allHeaderFields]);

                          response->data = String(static_cast<const char *>(nsData.bytes), nsData.length);

                          if (response->originalRequest.doneHandler) {
                              response->originalRequest.doneHandler(response);
                          }
                      });
                    }];

                if (dataTask != nullptr) {
                    [dataTask resume];
                }
            }
        }
    }
}
