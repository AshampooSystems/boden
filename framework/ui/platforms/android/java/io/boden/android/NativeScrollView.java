package io.boden.android;

import android.content.Context;
import androidx.core.widget.NestedScrollView;
import android.view.View;
import android.util.AttributeSet;
import android.widget.HorizontalScrollView;

/**
 * Creates and maintains a bi-directional scroll view that can scroll horizontally
 * AND vertically.
 *
 * The manager creates multiple nested views to achieve the desired effect.
 * It is designed to be controlled by a bdn::android::ScrollViewCore native component.
 *
 */
public class NativeScrollView
{
    public NativeScrollView(Context context)
    {
        _vertScrollView = new VerticalScrollView(context);

        _horzScrollView = new HorizontalScrollView(context);
        _vertScrollView.addView(_horzScrollView);

        _vertScrollView.setOnScrollChangeListener( new VerticalScrollChangeListener(this) );
        _horzScrollView.setOnScrollChangeListener( new HorizontalScrollChangeListener(this) );

        _contentParent = new io.boden.android.NativeViewGroup(context);
        _horzScrollView.addView(_contentParent);
    }

    /** Returns the outermost view object that wraps the whole scroll view.
     *
     *  This view can contain several nested sub views. Use getContentParent()
     *  to get the view that acts as the parent for the content view.
     * */
    public View getWrapperView()
    {
        return _vertScrollView;
    }


    /** Returns the ViewGroup that acts as the parent for the scroll view's
     *  content view. The content view should be the only child of this view.
     */
    public io.boden.android.NativeViewGroup getContentParent()
    {
        return _contentParent;
    }


    /** Scrolls to the specified scroll position (without animation)*/
    public void scrollTo (int x,
                          int y)
    {
        _vertScrollView.scrollTo(0, y);
        _horzScrollView.scrollTo(x, 0);
    }

    /** Scrolls to the specified scroll position with a smooth animation*/
    public void smoothScrollTo (int x,
                                int y)
    {
        _vertScrollView.smoothScrollTo(0, y);
        _horzScrollView.smoothScrollTo(x, 0);
    }


    /** Returns the X coordinate of the scroll position.*/
    public int getScrollX()
    {
        return _horzScrollView.getScrollX();
    }


    /** Returns the Y coordinate of the scroll position.*/
    public int getScrollY()
    {
        return _vertScrollView.getScrollY();
    }


    /** Returns the width of the scroll view.*/
    public int getWidth()
    {
        return _vertScrollView.getWidth();
    }

    /** Returns the height of the scroll view.*/
    public int getHeight()
    {
        return _vertScrollView.getHeight();
    }

    private native static void scrollChange(View wrapperView, int scrollX, int scrollY, int oldScrollX, int oldScrollY);

    private void verticalScrollChange(int scrollY, int oldScrollY)
    {
        int scrollX = _horzScrollView.getScrollX();

        scrollChange(_vertScrollView, scrollX, scrollY, scrollX, oldScrollY);
    }

    private void horizontalScrollChange(int scrollX, int oldScrollX)
    {
        int scrollY = _horzScrollView.getScrollY();

        scrollChange(_vertScrollView, scrollX, scrollY, oldScrollX, scrollY);
    }

    private class VerticalScrollChangeListener implements View.OnScrollChangeListener
    {
        VerticalScrollChangeListener( NativeScrollView scrollView )
        {
            _scrollView = scrollView;
        }

        public void onScrollChange (View view,
                                    int scrollX,
                                    int scrollY,
                                    int oldScrollX,
                                    int oldScrollY)
        {
            _scrollView.verticalScrollChange(scrollY, oldScrollY);
        }

        private NativeScrollView _scrollView;
    }

    private class HorizontalScrollChangeListener implements View.OnScrollChangeListener
    {
        HorizontalScrollChangeListener( NativeScrollView scrollView )
        {
            _scrollView = scrollView;
        }

        public void onScrollChange (View view,
                                    int scrollX,
                                    int scrollY,
                                    int oldScrollX,
                                    int oldScrollY)
        {
            _scrollView.horizontalScrollChange(scrollX, oldScrollX);
        }

        private NativeScrollView _scrollView;
    }

    private class VerticalScrollView extends NestedScrollView
    {
        public VerticalScrollView(Context context)
        {
            super(context);
        }

        public VerticalScrollView(Context context, AttributeSet attrs)
        {
            super(context, attrs);
        }

        public VerticalScrollView(Context context, AttributeSet attrs, int defStyleAttr)
        {
            super(context, attrs, defStyleAttr);
        }


        protected void onLayout (boolean changed,
                                 int left,
                                 int top,
                                 int right,
                                 int bottom)
        {
            if(getChildCount()>0)
            {
                View child = getChildAt(0);

                // the child view is the horizontal scroll view.
                // Make it the same width as us, and set its height to its
                // desired height
                child.layout(
                        0,
                        0,
                        right-left,
                        _contentParent.getMeasuredHeight() );
            }
        }
    }

    private VerticalScrollView _vertScrollView;
    private HorizontalScrollView _horzScrollView;
    private io.boden.android.NativeViewGroup _contentParent;
};
