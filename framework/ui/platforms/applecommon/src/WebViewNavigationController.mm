#import <bdn/applecommon/WebViewNavigationController.hh>

#include <bdn/foundationkit/conversionUtil.hh>
#include <bdn/log.h>

@implementation WebViewNavigationController

- (void)webView:(WKWebView *)webView didFailNavigation:(WKNavigation *)navigation withError:(NSError *)error
{
    bdn::logstream() << "Navigation failed: " << bdn::fk::nsStringToString(error.localizedDescription);
}

- (void)webView:(WKWebView *)webView didFailProvisionalNavigation:(WKNavigation *)navigation withError:(NSError *)error
{
    bdn::logstream() << "Provisional navigation failed: " << bdn::fk::nsStringToString(error.localizedDescription);
}

- (void)webView:(WKWebView *)webView
    decidePolicyForNavigationAction:(WKNavigationAction *)navigationAction
                    decisionHandler:(void (^)(WKNavigationActionPolicy))decisionHandler
{
    bdn::ui::WebView::RedirectRequest request;
    request.url = bdn::fk::nsStringToString(navigationAction.request.URL.absoluteString);

    if (!self.redirectHandler || self.redirectHandler(request)) {
        decisionHandler(WKNavigationActionPolicyAllow);
    } else {
        decisionHandler(WKNavigationActionPolicyCancel);
    }
}

@end
