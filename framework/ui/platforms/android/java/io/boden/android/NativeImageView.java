package io.boden.android;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;

import androidx.appcompat.widget.AppCompatImageView;

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
                URI aUri = null;
                try {
                    aUri = new URI(params[0]);
                } catch (URISyntaxException e) {
                    return bitmap;
                }

                if(aUri.getScheme().equals("resource")) {
                    int resId = NativeRootActivity.getRootActivity().getResourceIdFromURI(params[0], "drawable");
                    if(resId != -1) {
                        bitmap = BitmapFactory.decodeResource(NativeRootActivity.getRootActivity().getResources(), resId );
                    }
                }
                else {
                    URL url = new URL(params[0]);
                    bitmap = BitmapFactory.decodeStream((InputStream) url.getContent());
                }
            } catch (IOException e) {
                e.printStackTrace();
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