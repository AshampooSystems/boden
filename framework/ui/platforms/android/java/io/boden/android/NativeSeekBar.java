package io.boden.android;

import android.content.Context;
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

    private native void native_valueChanged(int newValue);
}