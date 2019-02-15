#include <bdn/AppRunnerBase.h>
#include <bdn/HTTP.h>
#include <bdn/HTTPRequest.h>
#include <bdn/HTTPResponse.h>
#include <bdn/config.h>

#ifdef BDN_PLATFORM_OSX
#import <bdn/mac/util.hh>
using namespace bdn::mac;
#else
#import <bdn/ios/util.hh>
using namespace bdn::ios;
#endif

#import <Foundation/Foundation.h>

namespace bdn
{
    namespace net
    {
        namespace http
        {
            std::shared_ptr<HTTPResponse> request(HTTPRequest request)
            {
                std::shared_ptr<HTTPResponse> response = std::make_shared<HTTPResponse>();

                response->originalRequest = request;

                NSURLSession *session = [NSURLSession sharedSession];
                NSURL *nsURL = [NSURL URLWithString:stringToNSString(request.url)];

                NSURLSessionDataTask *dataTask = [session
                      dataTaskWithURL:nsURL
                    completionHandler:^(NSData *_Nullable nsData, NSURLResponse *_Nullable nsResponse,
                                        NSError *_Nullable error) {
                      getMainDispatcher()->enqueue([nsData, nsResponse, response]() {
                          NSHTTPURLResponse *nsHTTPResponse = (NSHTTPURLResponse *)nsResponse;

                          response->url = nsStringToString([nsHTTPResponse.URL absoluteString]);
                          response->responseCode = (int)nsHTTPResponse.statusCode;

                          response->header =
                              nsStringToString([NSString stringWithFormat:@"%@", nsHTTPResponse.allHeaderFields]);

                          response->data =
                              String((const char *)nsData.bytes, (const char *)nsData.bytes + nsData.length);

                          if (response->originalRequest.doneHandler) {
                              response->originalRequest.doneHandler(response);
                          }
                      });
                    }];

                [dataTask resume];

                return response;
            }
        }
    }
}
