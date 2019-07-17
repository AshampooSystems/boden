package io.boden.android;

import android.content.Context;
import android.os.Build;
import android.widget.SeekBar;

import androidx.appcompat.widget.AppCompatSeekBar;

public class NativeSeekBar extends AppCompatSeekBar
{
    public NativeSeekBar(Context context) {
        super(context);

        setOnSeekBarChangeListener(new AppCompatSeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                native_valueChanged(progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
    }

    public void setMin(int minimum) {
        assert minimum == 0 : "Cannot change minimum yet, as Android < 26 does not support setMin(I)";

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            super.setMin(minimum);
        } else {
            // TODO: Implement backwards compatibility
        }
    }

    private native void native_valueChanged(int newValue);
}