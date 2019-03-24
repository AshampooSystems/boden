package io.boden.android;

import android.animation.Animator;
import android.content.Context;
import android.util.Log;

import com.airbnb.lottie.LottieAnimationView;
import com.airbnb.lottie.LottieListener;

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

    public void loadFromResource(String url) {
        int resId = NativeRootActivity.getRootActivity().getResourceIdFromURI(url, "raw");

        if(resId != -1) {
            this.setAnimation(resId);
        }

    }

    private native void onAnimationEnded();
}
