#include <bdn/ui/WebView.h>

#import <bdn/mac/WebViewCore.hh>
#import <bdn/mac/util.hh>

#import <WebKit/WebKit.h>
#include <bdn/mac/util.hh>

#include <bdn/log.h>

#include <bdn/ui/ViewUtilities.h>

@interface WebViewUIDelegate : NSObject <WKUIDelegate>

@end

@implementation WebViewUIDelegate
- (void)webView:(WKWebView *)webView
    runOpenPanelWithParameters:(WKOpenPanelParameters *)parameters
              initiatedByFrame:(WKFrameInfo *)frame
             completionHandler:(void (^)(NSArray<NSURL *> *URLs))completionHandler
{
    auto openPanel = [NSOpenPanel openPanel];

    openPanel.canChooseFiles = true;
    [openPanel beginWithCompletionHandler:^(NSModalResponse result) {
      if (result == NSModalResponseOK) {
          if (auto url = openPanel.URLs)
              completionHandler(url);
      }
    }];
}

@end

namespace bdn::ui::detail
{
    CORE_REGISTER(WebView, bdn::ui::mac::WebViewCore, WebView)
}

namespace bdn::ui::mac
{
    WKWebView *createWKWebView()
    {
        WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
        return [[WKWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    }

    WebViewCore::WebViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createWKWebView())
    {}

    void WebViewCore::init()
    {
        _navigationController = [[WebViewNavigationController alloc] init];
        _uiDelegate = [[WebViewUIDelegate alloc] init];
        ((WKWebView *)nsView()).navigationDelegate = _navigationController;
        ((WKWebView *)nsView()).UIDelegate = _uiDelegate;

        redirectHandler.onChange() += [=](auto &property) { _navigationController.redirectHandler = property.get(); };

        userAgent.onChange() +=
            [=](auto &property) { ((WKWebView *)nsView()).customUserAgent = fk::stringToNSString(property.get()); };
    }

    void WebViewCore::loadURL(const std::string &url)
    {
        WKWebView *webView = (WKWebView *)nsView();
        NSURL *nsURL = [[NSURL alloc] initWithString:bdn::fk::stringToNSString(url)];
        if (nsURL != nullptr) {
            NSURLRequest *request = [[NSURLRequest alloc] initWithURL:nsURL];
            if (request != nullptr) {
                [webView loadRequest:request];
            }
        }
    }
}
