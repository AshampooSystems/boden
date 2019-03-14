#include <bdn/WebView.h>

#import <bdn/mac/WebViewCore.hh>
#import <bdn/mac/util.hh>

#import <WebKit/WebKit.h>
#include <bdn/mac/util.hh>

#include <bdn/log.h>

#include <bdn/UIUtil.h>

namespace bdn::webview::detail
{
    CORE_REGISTER(WebView, bdn::mac::WebViewCore, WebView)
}

namespace bdn::mac
{
    WKWebView *createWKWebView()
    {
        WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
        return [[WKWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    }

    WebViewCore::WebViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, createWKWebView())
    {}

    void WebViewCore::init()
    {
        _navigationController = [[WebViewNavigationController alloc] init];
        ((WKWebView *)nsView()).navigationDelegate = _navigationController;

        redirectHandler.onChange() += [=](auto va) { _navigationController.redirectHandler = va->get(); };

        userAgent.onChange() +=
            [=](auto va) { ((WKWebView *)nsView()).customUserAgent = fk::stringToNSString(va->get()); };
    }

    void WebViewCore::loadURL(const String &url)
    {
        WKWebView *webView = (WKWebView *)nsView();
        NSURL *nsURL = [[NSURL alloc] initWithString:bdn::fk::stringToNSString(url)];
        NSURLRequest *request = [[NSURLRequest alloc] initWithURL:nsURL];
        [webView loadRequest:request];
    }
}
