package io.boden.android;

import android.content.Context;

import io.boden.java.NativeStrongPointer;

public class RowContainer extends io.boden.android.NativeViewGroup {
    public RowContainer(Context context) {
        super(context);
    }

    public NativeStrongPointer getBdnView() {
        return _bdnViewPtr;
    }
    public void setBdnView(NativeStrongPointer bdnViewPtr) {
        _bdnViewPtr = bdnViewPtr;
    }

    private NativeStrongPointer _bdnViewPtr;
}
