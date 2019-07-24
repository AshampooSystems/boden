package io.boden.android;

import android.content.Context;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.method.LinkMovementMethod;
import android.text.method.MovementMethod;
import android.view.MotionEvent;

import androidx.appcompat.widget.AppCompatTextView;

public class NativeLabel extends AppCompatTextView
{
    public NativeLabel(Context context) {
        super(context);
    }

    @Override
    public void setText(CharSequence text, BufferType type) {
        super.setText(text, type);

        if(text instanceof Spanned) {
            setMovementMethod(LinkMovementMethod.getInstance());
        } else {
            setMovementMethod(null);
        }
    }

}