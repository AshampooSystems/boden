package io.boden.android;

import android.os.Parcel;
import android.text.ParcelableSpan;
import android.text.TextPaint;
import android.text.style.MetricAffectingSpan;

import androidx.annotation.NonNull;

class BaselineShiftSpan extends MetricAffectingSpan
{
    BaselineShiftSpan(int offset) {
        _offset = offset;
    }

    @Override
    public void updateMeasureState(@NonNull TextPaint textPaint) {
        textPaint.baselineShift += _offset;
    }

    @Override
    public void updateDrawState(@NonNull TextPaint textPaint) {
        textPaint.baselineShift += _offset;
    }

    private int _offset;
}