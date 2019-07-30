package io.boden.android;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;

import androidx.appcompat.widget.AppCompatImageView;
import io.boden.android.NativeRootActivity;

import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;

public class NativeImageView extends AppCompatImageView implements io.boden.android.LoadImageToViewTaskCallback
{
    private Bitmap mBitmap;

    public NativeImageView(Context context) {
        super(context);
        this.setScaleType(ScaleType.FIT_XY);
    }

    public int getImageWidth() {
        if(mBitmap != null) {
            return mBitmap.getWidth();
        }
        return 0;
    }

    public int getImageHeight() {
        if(mBitmap != null) {
            return mBitmap.getHeight();
        }
        return 0;
    }

    public void loadUrl(String url) {
        mBitmap = null;

        if(this.loadImageTask != null) {
            loadImageTask.cancel(true);
        }
        loadImageTask = new io.boden.android.LoadImageToViewTask(this);
        loadImageTask.execute(url);
    }

    private io.boden.android.LoadImageToViewTask loadImageTask;

    private native void native_imageLoaded(int width, int height);

    @Override
    public void imageLoaded(Bitmap bitmap) {
        mBitmap = bitmap;
        setImageBitmap(bitmap);
        native_imageLoaded(getImageWidth(), getImageHeight());
        NativeRootActivity.nativeViewNeedsLayout(this);
    }
}