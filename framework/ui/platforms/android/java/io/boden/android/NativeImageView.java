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

public class NativeImageView extends AppCompatImageView
{
    private Bitmap mBitmap;

    public NativeImageView(Context context) {
        super(context);
        this.setScaleType(ScaleType.FIT_XY);
    }

    public class AsyncTaskLoadImage extends AsyncTask<String, String, Bitmap> {
        private AppCompatImageView imageView;

        public AsyncTaskLoadImage(AppCompatImageView imageView) {
            this.imageView = imageView;
        }

        @Override
        protected Bitmap doInBackground(String... params) {
            Bitmap bitmap = null;

            try {
                InputStream stream = NativeRootActivity.getStreamFromURI(params[0]);
                if(stream != null) {
                    bitmap = BitmapFactory.decodeStream(stream);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            return bitmap;
        }

        @Override
        protected void onPostExecute(Bitmap bitmap) {
            try {
                mBitmap = bitmap;
                imageView.setImageBitmap(bitmap);

                native_imageLoaded(getImageWidth(), getImageHeight());
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
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
            this.loadImageTask.cancel(true);
        }
        this.loadImageTask = new AsyncTaskLoadImage(this);
        loadImageTask.execute(url);
    }

    private AsyncTaskLoadImage loadImageTask;

    private native void native_imageLoaded(int width, int height);
}