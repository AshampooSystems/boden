package io.boden.android;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.webkit.WebChromeClient;
import android.webkit.WebView;

public class NativeWebView extends WebView {
    public NativeWebView(Context context) {
        super(context);

        _nativeClient = new NativeWebViewClient();
        this.setWebViewClient(_nativeClient);
        getSettings().setJavaScriptEnabled(true);
    }

    public void setUserAgent(String userAgent) {
        this.getSettings().setUserAgentString(userAgent);
    }


    private NativeWebViewClient _nativeClient;
}
