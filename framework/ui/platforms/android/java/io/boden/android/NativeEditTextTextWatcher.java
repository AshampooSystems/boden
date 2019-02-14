package io.boden.android;

import android.text.TextWatcher;
import android.text.Editable;
import android.widget.TextView;

public class NativeEditTextTextWatcher extends Object implements TextWatcher
{
    public NativeEditTextTextWatcher(TextView view)
    {
    	_view = view;
    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
    	nativeBeforeTextChanged(_view, s.toString(), start, count, after);
    }

    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {
    	nativeOnTextChanged(_view, s.toString(), start, before, count);
    }

    @Override
    public void afterTextChanged(Editable s) {
    	nativeAfterTextChanged(_view);
    }

    public native void nativeBeforeTextChanged(TextView view, String string, int start, int count, int after);
    public native void nativeOnTextChanged(TextView view, String string, int start, int before, int count);
    public native void nativeAfterTextChanged(TextView view);

    private TextView _view;
}
