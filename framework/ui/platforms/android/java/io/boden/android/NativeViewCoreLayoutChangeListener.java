package io.boden.android;

import android.view.View;


/** An OnClickListener implementation to be attached to java View objects.
 *  The click events will be forwarded to native code View::Core objects that are
 *  associated with the view.
 *
 *  The ViewCore native object must be wrapped in a NativeWeakPointer
 *  object which in turn is set as the tag object (see View::setTag) of the view
 *
 *  */
public class NativeViewCoreLayoutChangeListener extends Object implements View.OnLayoutChangeListener
{
    public NativeViewCoreLayoutChangeListener()
    {
    }

    public void onLayoutChange(View view, int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight, int oldBottom)
    {
        viewLayoutChange(view, left, top, right, bottom, oldLeft, oldTop, oldRight, oldBottom);
    }

    public native void viewLayoutChange(View view, int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight, int oldBottom);

}
