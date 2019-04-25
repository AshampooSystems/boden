package io.boden.android;

import android.animation.Animator;
import android.content.Context;
import android.net.Uri;
import android.util.JsonReader;
import android.util.Log;
import android.webkit.MimeTypeMap;

import com.airbnb.lottie.LottieAnimationView;
import com.airbnb.lottie.LottieListener;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.reflect.Field;
import java.net.URI;
import java.net.URISyntaxException;

public class NativeLottieAnimationView extends LottieAnimationView {
    public NativeLottieAnimationView(Context context) {
        super(context);

        this.addAnimatorListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {

            }

            @Override
            public void onAnimationEnd(Animator animation) {
                onAnimationEnded();
            }

            @Override
            public void onAnimationCancel(Animator animation) {

            }

            @Override
            public void onAnimationRepeat(Animator animation) {

            }
        });
    }

    public void loadFromUrl(String url) {

        InputStream stream = io.boden.android.NativeRootActivity.getStreamFromURI(url);

        if(stream != null) {
            try {
                JsonReader jsonReader = new JsonReader(new InputStreamReader(stream, "UTF-8"));
                this.setAnimation(jsonReader, url);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private native void onAnimationEnded();
}
