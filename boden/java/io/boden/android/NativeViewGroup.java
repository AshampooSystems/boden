package io.boden.android;


import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

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
    }

    public void setChildBounds(View child, int x, int y, int width, int height)
    {
        // modify the child's layout params

        final LayoutParams params = (LayoutParams)child.getLayoutParams();

        params.x = x;
        params.y = y;
        params.width = width;
        params.height = height;


        // and then schedule a re-layout of this container so that the child will be repositioned
        if(!isInLayout())
            requestLayout();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        setMeasuredDimension(
                resolveSize(mWidth, widthMeasureSpec),
                resolveSize(mHeight, heightMeasureSpec));
    }


    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        final int childCount = getChildCount();

        for(int i=0; i<childCount; i++)
        {
            final View child = getChildAt(i);

            if(child.getVisibility() != GONE)
            {
                final LayoutParams params = (LayoutParams)child.getLayoutParams();

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

}
