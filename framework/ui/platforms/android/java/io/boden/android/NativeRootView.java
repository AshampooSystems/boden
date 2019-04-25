package io.boden.android;


import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toolbar;

/**
 * Root view group that contains user interface components that are controlled by native code.
 * Objects of this class are connected to a "Window" object in native code.
 *
 * The purpose of this class is to allow embedding of native code-controlled views inside an
 * existing Android activity. If you want an entire activity to be native controlled then
 * you should use NativeRootActivity instead.
 *
 * The root view must be informed of certain events (like rotation changes, etc.) to ensure
 * that the native code side works properly. The onConfigurationChanged method exists for this purpose.
 * It works the same way as the one defined in Activity, so we recommend that you call the root view's
 * onConfigurationChanged directly from your Activity's onConfigurationChanged.
 *
 * If you use NativeRootActivity instead of NativeRootView then you do not have to be concerned
 * with configuration notifications. They are forwarded automatically.
 *
 *
 */
public class NativeRootView extends NativeViewGroup
{
    public NativeRootView(Context context)
    {
        super(context);

        created();
    }

    public NativeRootView(Context context, AttributeSet attrs)
    {
        this(context, attrs, 0);

        created();
    }

    public NativeRootView(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);

        created();
    }

    public void setTitle(CharSequence title)
    {
        ((Activity)getContext()).getActionBar().setTitle(title);
    }

    public void enableBackButton(boolean enabled) {
        ((Activity)getContext()).getActionBar().setDisplayHomeAsUpEnabled(enabled);
    }


    /** Notifies the root view that the device's configuration has changed
     *  (screen orientation, screen size, etc.). See NativeRootView class description
     *  for more information.
     */
    public void onConfigurationChanged (Configuration newConfig)
    {
        configurationChanged(newConfig);

        super.onConfigurationChanged(newConfig);
    }



    /** Dispose of this root view. Releases the object's resources and ensures
     *  that it is not used to display native code views anymore.
      */
    void dispose()
    {
        if(!_disposed)
        {
            disposed();

            _disposed = true;
        }

        // clear the tag object that that any associated C++ object is released.
        setTag( null );
    }


    @Override
    public void finalize() throws Throwable
    {
        dispose();

        super.finalize();
    }


    public static String padLeft(String s, int n) {
        String result = "";

        for(int i=0;i<n;i++) {
            result += " ";
        }

        result += s;
        return result;
    }

    @Override
    protected void onMeasure(int w, int h) {
        super.onMeasure(w,h);
        doLayout(false, 0, 0, 0, 0);
    }

/*
    protected void printInfo(View v, int depth) {
        Log.i("boden",  padLeft(v.toString() + ": " + Integer.toString((int) v.getX()) + "x" + Integer.toString((int) v.getY()) +
                ", " + Integer.toString((int) v.getWidth()) + "x" + Integer.toString((int) v.getHeight()), depth));

        if(v instanceof ViewGroup) {
            ViewGroup vGroup = ((ViewGroup) v);
            final int childCount = vGroup.getChildCount();
            for(int i=0; i<childCount; i++) {
                final View child = vGroup.getChildAt(i);
                printInfo(child, depth+1);
            }
        }
    }
*/
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);

        if(getChildAt(0) != null) {
            getChildAt(0).layout(0, 0, right, bottom);
        }
        //printInfo(this, 0);
    }

    @Override
    public void onSizeChanged(int newWidth, int newHeight, int oldWidth, int oldHeight)
    {
        sizeChanged(newWidth, newHeight);
    }

    public boolean handleBackPressed() {
        return native_handleBackPressed();
    }

    private native void created();
    private native void disposed();
    private native void sizeChanged(int width, int height);
    private native void configurationChanged(Configuration newConfig);

    private native boolean native_handleBackPressed();

    private boolean _disposed = false;
};



