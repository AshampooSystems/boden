package io.boden.android;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.util.TypedValue;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatButton;
import androidx.appcompat.widget.AppCompatImageButton;

import java.io.InputStream;

import io.boden.android.NativeViewGroup;

class NativeButton extends ViewGroup implements io.boden.android.LoadImageToViewTaskCallback, View.OnClickListener
{
    private AppCompatButton _textButton;
    private AppCompatImageButton _imageButton;
    private io.boden.android.LoadImageToViewTask _loadImageTask;
    private OnClickListener _onClickListener;

    public NativeButton(Context context) {

        super(context);

        _textButton = new AppCompatButton(context);
        _textButton.setSingleLine(true);
        _textButton.setOnClickListener(this);

        _imageButton = new AppCompatImageButton(context);
        _imageButton.setVisibility(INVISIBLE);
        _imageButton.setOnClickListener(this);

        TypedValue outValue = new TypedValue();
        getContext().getTheme().resolveAttribute(android.R.attr.selectableItemBackgroundBorderless, outValue, true);
        _imageButton.setBackgroundResource(outValue.resourceId);

        addView(_textButton);
        addView(_imageButton);
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        if(changed) {
            _textButton.layout(0, 0, r-l, b-t);
            _imageButton.layout(0, 0, r-l, b-t);

            if(_textButton.getVisibility() == VISIBLE) {
            } else if(_imageButton.getVisibility() == VISIBLE) {
            }
        }
    }

    @Override
    public int getBaseline() {
        if(_textButton.getVisibility() == VISIBLE) {
            return _textButton.getBaseline();
        } else {
            return _imageButton.getBaseline();
        }
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        View activeBtn = _textButton;

        if(_imageButton.getVisibility() == VISIBLE) {
            activeBtn = _imageButton;
        }

        activeBtn.measure(widthMeasureSpec, heightMeasureSpec);
        setMeasuredDimension(activeBtn.getMeasuredWidth(), activeBtn.getMeasuredHeight());
    }

    public void setText(CharSequence charSequence) {
        _textButton.setVisibility(VISIBLE);
        _imageButton.setVisibility(INVISIBLE);
        _textButton.setText(charSequence);
    }

    public boolean setImage(String url)
    {
        _textButton.setVisibility(INVISIBLE);
        _imageButton.setVisibility(VISIBLE);

        if(_loadImageTask != null) {
            _loadImageTask.cancel(true);
        }
        _loadImageTask = new io.boden.android.LoadImageToViewTask(this);
        _loadImageTask.execute(url);

        return false;
    }

    @Override
    public void imageLoaded(Bitmap bitmap) {
        _imageButton.setImageBitmap(bitmap);
        io.boden.android.NativeRootActivity.nativeViewNeedsLayout(this);
    }

    @Override
    public void onClick(View v) {
        if(_onClickListener != null) {
            _onClickListener.onClick(this);
        }
    }

    @Override
    public void setOnClickListener(@Nullable OnClickListener l) {
        _onClickListener = l;
    }
}