package io.boden.android;

import android.app.Activity;
import android.content.Context;

public class NativeStackView extends NativeViewGroup implements NativeRootActivity.BackButtonListener {
    public NativeStackView(Context context) {
        super(context);

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

    @Override
    public boolean backButtonPressed() {
        return handleBackButton();
    }

    private native boolean handleBackButton();
}
