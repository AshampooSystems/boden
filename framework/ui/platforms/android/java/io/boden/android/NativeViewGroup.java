package io.boden.android;


import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.HorizontalScrollView;
import android.widget.ScrollView;

public class NativeViewGroup extends ViewGroup
{
    private int mWidth=0;
    private int mHeight=0;

    public NativeViewGroup(Context context)
    {
        super(context);
    }

    public NativeViewGroup(Context context, AttributeSet attrs)
    {
        this(context, attrs, 0);
    }

    public NativeViewGroup(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
    }

    @Override
    public boolean shouldDelayChildPressedState()
    {
        return false;
    }


    /** Sets the an object that contains data that is needed by the native code side of this object.
     *  This function is used internally and should usually not be called.*/
    public void _setNativeData( Object o)
    {
        mNativeObject = o;
    }

    /** Returns the native data object that is associated with this Java object.*/
    public Object getNativeData()
    {
        return mNativeObject;
    }

    public void setSize(int width, int height)
    {
        mWidth = width;
        mHeight = height;

        setMeasuredDimension(mWidth,mHeight);
    }

    public void setChildBounds(View child, int x, int y, int width, int height)
    {
        // modify the child's layout params

        // Log.i("boden", "setChildBounds " + child.toString()+" to "+Integer.toString(width)+"x"+Integer.toString(height));

        final LayoutParams params = new LayoutParams(x, y, width, height);
        child.setLayoutParams(params);

        // and then schedule a re-layout of this container so that the child will be repositioned
        if(isInLayout())
        {
            // Log.i("boden", "skipped requestLayout");
        }
        else
        {
            // Log.i("boden", "requestLayout " + toString());
            requestLayout();
        }
    }

    public void addView (View child)
    {
        super.addView(child);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        // note that we MUST call measure on our children. Otherwise their
        // onLayout method will not be called when we call their layout method from our onLayout.
        final int childCount = getChildCount();
        for(int i=0; i<childCount; i++) {
            final View child = getChildAt(i);
            child.measure(0, 0);
        }

        setMeasuredDimension(
                resolveSize(mWidth, widthMeasureSpec),
                resolveSize(mHeight, heightMeasureSpec));

        //super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }


    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        doLayout(changed, left, top, right, bottom);

        final int childCount = getChildCount();

        String myParams = Integer.toString(left)+", "+Integer.toString(top)+", "+Integer.toString(right)+", "+Integer.toString(bottom);

        /*if(childCount==0)
            Log.i("boden", getClass().getName()+".onLayout("+myParams+") with zero children.");
        else
            Log.i("boden", getClass().getName()+".onLayout("+myParams+") "+toString()+" started.");*/

        for(int i=0; i<childCount; i++)
        {
            final View child = getChildAt(i);

            if(child.getVisibility() != GONE)
            {
                final LayoutParams params = (LayoutParams)child.getLayoutParams();

                // Log.i("boden", "Laying out child " + child.toString()+" to size "+Integer.toString(params.width)+"x"+Integer.toString(params.height));

                child.layout( params.x, params.y, params.x+params.width, params.y+params.height );
            }
        }
    }


    @Override
    public LayoutParams generateLayoutParams(AttributeSet attrs)
    {
        return new LayoutParams(getContext(), attrs);
    }

    @Override
    protected LayoutParams generateDefaultLayoutParams()
    {
        return new LayoutParams(0, 0, 0, 0);
    }

    @Override
    protected ViewGroup.LayoutParams generateLayoutParams(ViewGroup.LayoutParams p)
    {
        return new LayoutParams(p);
    }

    @Override
    protected boolean checkLayoutParams(ViewGroup.LayoutParams p)
    {
        return p instanceof LayoutParams;
    }


    public static class LayoutParams extends ViewGroup.LayoutParams
    {
        public int x = 0;
        public int y = 0;

        final static String attrNamespace = "io.boden.ExternalLayoutViewGroup";
        final static String attrNameX = "x";
        final static String attrNameY = "y";


        public LayoutParams(Context c, AttributeSet attrs)
        {
            super(c, attrs);

            x = attrs.getAttributeIntValue(attrNamespace, attrNameX, 0);
            y = attrs.getAttributeIntValue(attrNamespace, attrNameY, 0);
        }

        public LayoutParams(int x, int y, int width, int height)
        {
            super(width, height);

            this.x = x;
            this.y = y;
        }

        public LayoutParams(ViewGroup.LayoutParams source)
        {
            super(source);

            if(source instanceof LayoutParams)
            {
                LayoutParams s = (LayoutParams)source;

                x = s.x;
                y = s.y;
            }
        }
    }

    private Object mNativeObject;

    public native void doLayout(boolean changed, int left, int top, int right, int bottom);

}
