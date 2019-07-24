package io.boden.android;

import android.text.style.ClickableSpan;
import android.text.style.SuperscriptSpan;
import android.view.View;

import androidx.annotation.NonNull;


class LinkClickSpan extends ClickableSpan {
    LinkClickSpan(Object attributedString, String url) {
        _attributedString = attributedString;
        _url = url;
    }

    @Override
    public void onClick(@NonNull View widget) {
        linkClicked(_attributedString, _url);
    }

    private native static void linkClicked(Object attributedString, String url);

    private Object _attributedString;
    private String _url;
}

