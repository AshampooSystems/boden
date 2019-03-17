package io.boden.android;

import android.animation.Animator;
import android.content.Context;

import com.airbnb.lottie.LottieAnimationView;

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

    private native void onAnimationEnded();
}
