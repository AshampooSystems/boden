package io.boden.android;

import android.view.View;


/** An OnClickListener implementation to be attached to java View objects.
 *  The click events will be forwarded to native code bdn::ViewCore objects that are
 *  associated with the view.
 *
 *  The ViewCore native object must be wrapped in a NativeWeakPointer
 *  object which in turn is set as the tag object (see View::setTag) of the view
 *
 *  */
public class NativeViewCoreClickListener extends Object implements View.OnClickListener
{
    public NativeViewCoreClickListener()
    {
    }

    public void	onClick(View view)
    {
        viewClicked(view);
    }

    public native void viewClicked(View view);

}
