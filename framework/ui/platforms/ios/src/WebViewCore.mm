#include <bdn/ui/ViewUtilities.h>
#include <bdn/ui/WebView.h>

#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/ios/WebViewCore.hh>

#import <WebKit/WebKit.h>

@interface BodenWebView : WKWebView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
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

namespace bdn::ui::detail
{
    CORE_REGISTER(WebView, bdn::ui::ios::WebViewCore, WebView)
}

namespace bdn::ui::ios
{
    BodenWebView *createWKWebView()
    {
        WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
        return [[BodenWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    }

    WebViewCore::WebViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createWKWebView())
    {}

    void WebViewCore::init()
    {
        ViewCore::init();

        _navigationController = [[WebViewNavigationController alloc] init];
        ((WKWebView *)uiView()).navigationDelegate = _navigationController;

        redirectHandler.onChange() += [=](auto &property) { _navigationController.redirectHandler = property.get(); };

        userAgent.onChange() +=
            [=](auto &property) { ((WKWebView *)uiView()).customUserAgent = fk::stringToNSString(property.get()); };
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
