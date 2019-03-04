#include <bdn/WebView.h>

#import <bdn/mac/WebViewCore.hh>
#import <bdn/mac/util.hh>

#import <WebKit/WebKit.h>

namespace bdn
{
    namespace mac
    {
        WKWebView *createWKWebView()
        {
            WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
            return [[WKWebView alloc] initWithFrame:CGRectZero configuration:configuration];
        }

        WebViewCore::WebViewCore(std::shared_ptr<WebView> outer) : ChildViewCore(outer, createWKWebView())
        {
            if (outer->url != "") {
                loadURL(outer->url);
            }
        }

        void WebViewCore::loadURL(const String &url)
        {
            WKWebView *webView = (WKWebView *)nsView();
            NSURL *nsURL = [[NSURL alloc] initWithString:bdn::mac::stringToNSString(url)];
            NSURLRequest *request = [[NSURLRequest alloc] initWithURL:nsURL];
            [webView loadRequest:request];
        }
    }
}
