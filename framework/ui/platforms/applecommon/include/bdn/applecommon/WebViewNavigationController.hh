#pragma once

#import <WebKit/WebKit.h>

#include "../../../include/bdn/WebView.h"

@interface WebViewNavigationController : NSObject <WKNavigationDelegate>
@property std::function<bool(bdn::WebView::RedirectRequest)> redirectHandler;
//- initWithCore:(std::shared_ptr<bdn::mac::WebViewCore>)core;
@end
