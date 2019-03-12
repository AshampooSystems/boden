package io.boden.android;

import android.animation.Animator;
import android.app.Activity;
import android.content.Context;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.transition.AutoTransition;
import android.transition.Explode;
import android.transition.Fade;
import android.transition.Scene;
import android.transition.Slide;
import android.transition.Transition;
import android.transition.TransitionManager;
import android.transition.TransitionSet;
import android.transition.TransitionValues;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;

public class NativeStackView extends NativeViewGroup implements NativeRootActivity.BackButtonListener {

    public NativeStackView(Context context) {
        super(context);

        int id = View.generateViewId();
        setId(id);

        shortAnimationDuration = getResources().getInteger(
                android.R.integer.config_shortAnimTime);

        Slide enterSlide = new Slide(Gravity.RIGHT);
        enterSlide.setDuration(shortAnimationDuration);
        enterAnimation = enterSlide;

        Slide returnSlide = new Slide(Gravity.LEFT);
        returnSlide.setDuration(shortAnimationDuration);
        returnAnimation = returnSlide;

        exitAnimation = new AutoTransition();

        NativeRootActivity.getRootActivity().addBackButtonListener(this);
    }

    public void close() {
        ((NativeRootActivity)getContext()).removeBackButtonListener(this);
    }

    public void setWindowTitle(String title) {
        NativeRootActivity.getRootActivity().getActionBar().setTitle(title);
    }

    public void enableBackButton(boolean enabled) {
        NativeRootActivity.getRootActivity().getActionBar().setDisplayHomeAsUpEnabled(enabled);
    }

    public void changeContent(View newContent, boolean animate, boolean isEnter) {

        NativeStackFragment newFragment = new NativeStackFragment();
        newFragment.setView(newContent);
        newContent.requestLayout();

        FragmentManager fragmentManager = NativeRootActivity.getRootActivity().getSupportFragmentManager();

        FragmentTransaction transaction = fragmentManager.beginTransaction();

        int myId = this.getId();

        if(animate) {
            if(isEnter) {
                newFragment.setEnterTransition(enterAnimation);
            } else {
                newFragment.setEnterTransition(returnAnimation);
            }

            if(currentFragment != null) {
                currentFragment.setExitTransition(exitAnimation);
            }
        }

        transaction.replace(myId, newFragment);
        transaction.commitNow();

        currentFragment = newFragment;
    }

    @Override
    public boolean backButtonPressed() {
        return handleBackButton();
    }

    private NativeStackFragment currentFragment;
    private int shortAnimationDuration;
    private Transition enterAnimation;
    private Transition returnAnimation;
    private Transition exitAnimation;

    private native boolean handleBackButton();
}
