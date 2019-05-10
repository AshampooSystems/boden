#pragma once

#import <WebKit/WebKit.h>

#include <bdn/ui/WebView.h>

@interface WebViewNavigationController : NSObject <WKNavigationDelegate>
@property std::function<bool(bdn::ui::WebView::RedirectRequest)> redirectHandler;
//- initWithCore:(std::shared_ptr<bdn::mac::WebViewCore>)core;
@end
