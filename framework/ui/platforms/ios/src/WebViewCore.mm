#include <bdn/ViewUtilities.h>
#include <bdn/WebView.h>

#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/WebViewCore.hh>

#import <WebKit/WebKit.h>

@interface BodenWebView : WKWebView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

@implementation BodenWebView

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}

@end

namespace bdn::webview::detail
{
    CORE_REGISTER(WebView, bdn::ios::WebViewCore, WebView)
}

namespace bdn::ios
{
    BodenWebView *createWKWebView()
    {
        WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
        return [[BodenWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    }

    WebViewCore::WebViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createWKWebView())
    {}

    void WebViewCore::init()
    {
        ViewCore::init();

        _navigationController = [[WebViewNavigationController alloc] init];
        ((WKWebView *)uiView()).navigationDelegate = _navigationController;

        redirectHandler.onChange() += [=](auto va) { _navigationController.redirectHandler = va->get(); };

        userAgent.onChange() +=
            [=](auto va) { ((WKWebView *)uiView()).customUserAgent = fk::stringToNSString(va->get()); };
    }

    void WebViewCore::loadURL(const String &url)
    {
        auto webView = (WKWebView *)uiView();
        auto nsURL = [[NSURL alloc] initWithString:fk::stringToNSString(url)];
        if (nsURL != nullptr) {
            if (auto request = [[NSURLRequest alloc] initWithURL:nsURL]) {
                [webView loadRequest:request];
            }
        }
    }
}
