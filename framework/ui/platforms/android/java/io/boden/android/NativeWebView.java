package io.boden.android;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.util.Pair;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebStorage;
import android.webkit.WebView;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

import io.boden.android.NativeRootActivity;

import static android.app.Activity.RESULT_OK;
import static android.content.ContentValues.TAG;

public class NativeWebView extends WebView {
    public static final int INPUT_FILE_REQUEST_CODE = 1;
    private ValueCallback<Uri[]> mFilePathCallback;
    private String mCameraPhotoPath;

    private File createImageFile() throws IOException {
        // Create an image file name
        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
        String imageFileName = "JPEG_" + timeStamp + "_";
        File storageDir = getContext().getFilesDir();
        File imageFile = File.createTempFile(
                imageFileName,  /* prefix */
                ".jpg",         /* suffix */
                storageDir      /* directory */
        );
        return imageFile;
    }

    public NativeWebView(Context context) {
        super(context);

        WebSettings settings = getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDatabaseEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setJavaScriptEnabled(true);

        _nativeClient = new io.boden.android.NativeWebViewClient();
        this.setWebViewClient(_nativeClient);

        this.setWebChromeClient(new WebChromeClient() {
            public boolean onShowFileChooser(
                    WebView webView, ValueCallback<Uri[]> filePathCallback,
                    WebChromeClient.FileChooserParams fileChooserParams) {
                if(mFilePathCallback != null) {
                    mFilePathCallback.onReceiveValue(null);
                }
                mFilePathCallback = filePathCallback;

                String[] accTypes = fileChooserParams.getAcceptTypes();

                /*Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                if (takePictureIntent.resolveActivity(NativeRootActivity.getRootActivity().getPackageManager()) != null) {
                    // Create the File where the photo should go
                    File photoFile = null;
                    try {
                        photoFile = createImageFile();
                        takePictureIntent.putExtra("PhotoPath", mCameraPhotoPath);
                    } catch (IOException ex) {
                        // Error occurred while creating the File
                        Log.e(TAG, "Unable to create Image File", ex);
                    }

                    // Continue only if the File was successfully created
                    if (photoFile != null) {
                        mCameraPhotoPath = "file:" + photoFile.getAbsolutePath();
                        takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT,
                                Uri.fromFile(photoFile));
                    } else {
                        takePictureIntent = null;
                    }
                }*/

                Intent contentSelectionIntent = new Intent(Intent.ACTION_GET_CONTENT);
                contentSelectionIntent.addCategory(Intent.CATEGORY_OPENABLE);
                contentSelectionIntent.setType("*/*");

                Intent[] intentArray;
                /*if(takePictureIntent != null) {
                    intentArray = new Intent[]{takePictureIntent};
                } else*/
                {
                    intentArray = new Intent[0];
                }

                Intent chooserIntent = new Intent(Intent.ACTION_CHOOSER);
                chooserIntent.putExtra(Intent.EXTRA_INTENT, contentSelectionIntent);
                chooserIntent.putExtra(Intent.EXTRA_TITLE, "Choose .kdb(x)");
                chooserIntent.putExtra(Intent.EXTRA_INITIAL_INTENTS, intentArray);


                NativeRootActivity.getRootActivity().startActivityForResult(chooserIntent, new ValueCallback<Pair<Intent, Integer>>() {
                    @Override
                    public void onReceiveValue(Pair<Intent, Integer> value) {
                        if (null == mFilePathCallback)
                            return;

                        if (value.second == RESULT_OK) {
                            String dataString = value.first.getDataString();
                            if (dataString != null) {
                                mFilePathCallback.onReceiveValue(new Uri[]{Uri.parse(dataString)});
                            }

                            mFilePathCallback = null;
                        }
                    }
                });

                return true;
            }
        });

    }

    public void setUserAgent(String userAgent) {
        this.getSettings().setUserAgentString(userAgent);
    }


    private io.boden.android.NativeWebViewClient _nativeClient;
}
