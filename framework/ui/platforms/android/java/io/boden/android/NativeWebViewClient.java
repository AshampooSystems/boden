package io.boden.android;

import android.graphics.Bitmap;
import android.net.Uri;
import android.webkit.WebResourceRequest;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class NativeWebViewClient extends WebViewClient {
    @Override
    public boolean shouldOverrideUrlLoading (WebView view,
                                             WebResourceRequest request) {

        String s = request.getUrl().toString();

        if(s.startsWith("intent://")) {
            return true;
        }

        return false;
    }

    @Override
    public void onPageStarted(WebView view, String url, Bitmap favicon) {

    }

    @Override
    public void onPageFinished(WebView view, String url) {
        // Loading finished for URL
    }
}
