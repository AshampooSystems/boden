#include <bdn/WebView.h>

#import <bdn/ios/WebViewCore.hh>
#import <bdn/ios/util.hh>

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

namespace bdn::ios
{
    BodenWebView *createWKWebView()
    {
        WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
        return [[BodenWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    }

    WebViewCore::WebViewCore() : ViewCore(createWKWebView()) {}

    void WebViewCore::loadURL(const String &url)
    {
        WKWebView *webView = (WKWebView *)uiView();
        NSURL *nsURL = [[NSURL alloc] initWithString:bdn::ios::stringToNSString(url)];
        NSURLRequest *request = [[NSURLRequest alloc] initWithURL:nsURL];
        [webView loadRequest:request];
    }
}
