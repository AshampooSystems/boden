package io.boden.android;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.widget.ImageView;

import java.io.InputStream;

public class LoadImageToViewTask extends AsyncTask<String, String, Bitmap> {
    private io.boden.android.LoadImageToViewTaskCallback _callback;

    public LoadImageToViewTask(io.boden.android.LoadImageToViewTaskCallback callback) {
        _callback = callback;
    }

    @Override
    protected Bitmap doInBackground(String... params) {
        Bitmap bitmap = null;

        try {
            InputStream stream = io.boden.android.NativeRootActivity.getStreamFromURI(params[0]);
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
            _callback.imageLoaded(bitmap);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}