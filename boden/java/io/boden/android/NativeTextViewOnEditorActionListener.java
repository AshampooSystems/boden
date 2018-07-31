package io.boden.android;

import android.view.KeyEvent;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

public class NativeTextViewOnEditorActionListener extends Object implements TextView.OnEditorActionListener
{
    @Override
    public boolean onEditorAction(TextView textView, int actionId, KeyEvent event) {
    	return viewCoreOnEditorAction(textView, actionId, event);
    }

    public native boolean viewCoreOnEditorAction(TextView textView, int actionId, KeyEvent event);
}
