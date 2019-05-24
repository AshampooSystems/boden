package io.boden.android;

import android.content.Context;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

import android.transition.AutoTransition;
import android.transition.Slide;
import android.transition.Transition;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;

import java.util.ArrayList;
import java.util.List;

public class NativeNavigationView extends NativeViewGroup implements NativeRootActivity.BackButtonListener {

    public NativeNavigationView(Context context) {
        super(context);

        fragmentQueue = new ArrayList<NativeStackFragment>();

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
        NativeRootActivity.getRootView().setTitle(title);
    }

    public void enableBackButton(boolean enabled) {
        NativeRootActivity.getRootView().enableBackButton(enabled);
    }

    public void changeContent(View newContent, boolean animate, boolean isEnter) {
        NativeStackFragment newFragment = new NativeStackFragment();
        newFragment.setView(newContent);
        newContent.requestLayout();

        if (getParent() == null) {
            this.removeAllViews();
            this.addView(newContent);

            if(isEnter) {
                fragmentQueue.add(newFragment);
            } else {
                fragmentQueue.remove(fragmentQueue.size()-1);
            }
            return;
        }

        if (!fragmentQueue.isEmpty()) {
            this.removeAllViews();
            for (int i = 0; i < fragmentQueue.size(); i++) {
                doFragmentTransaction(fragmentQueue.get(i), false, true);
            }
            fragmentQueue.clear();
        }

        doFragmentTransaction(newFragment, animate, isEnter);
    }

    public void doFragmentTransaction(NativeStackFragment newFragment, boolean animate, boolean isEnter) {

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
        try {
            transaction.commitNow();
        } catch(IllegalArgumentException e) {
            return;
        }
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
    private List<NativeStackFragment> fragmentQueue;

    private native boolean handleBackButton();
}
