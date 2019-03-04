#include <bdn/WebView.h>

#import <bdn/ios/WebViewCore.hh>
#import <bdn/ios/util.hh>

#import <WebKit/WebKit.h>

@interface BodenWebView : WKWebView <UIViewWithFrameNotification>
@property(nonatomic, assign) bdn::ios::ViewCore *viewCore;
@end

@implementation BodenWebView

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}

@end

namespace bdn
{
    namespace ios
    {
        BodenWebView *createWKWebView()
        {
            WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
            return [[BodenWebView alloc] initWithFrame:CGRectZero configuration:configuration];
        }

        WebViewCore::WebViewCore(std::shared_ptr<WebView> outer) : ViewCore(outer, createWKWebView())
        {
            if (outer->url != "") {
                loadURL(outer->url);
            }
        }

        void WebViewCore::loadURL(const String &url)
        {
            WKWebView *webView = (WKWebView *)uiView();
            NSURL *nsURL = [[NSURL alloc] initWithString:bdn::ios::stringToNSString(url)];
            NSURLRequest *request = [[NSURLRequest alloc] initWithURL:nsURL];
            [webView loadRequest:request];
        }
    }
}
