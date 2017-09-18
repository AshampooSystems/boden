package io.boden.android;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ScrollView;
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
public class NativeScrollViewManager
{
    public NativeScrollViewManager(Context context)
    {
        _vertScrollView = new VerticalSubScrollView(context);

        _horzScrollView = new HorizontalScrollView(context);
        _vertScrollView.addView(_horzScrollView);

        _vertScrollView.setOnScrollChangeListener( new VerticalScrollChangeListener(this) );
        _horzScrollView.setOnScrollChangeListener( new HorizontalScrollChangeListener(this) );

        _contentParent = new NativeViewGroup(context);
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
    public NativeViewGroup getContentParent()
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
        int vw = _vertScrollView.getWidth();
        int hw = _horzScrollView.getWidth();
        int cw = _contentParent.getWidth();

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

    private void vertScrollChange(int scrollY, int oldScrollY)
    {
        int scrollX = _horzScrollView.getScrollX();

        scrollChange(_vertScrollView, scrollX, scrollY, scrollX, oldScrollY);
    }

    private void horzScrollChange(int scrollX, int oldScrollX)
    {
        int scrollY = _horzScrollView.getScrollY();

        scrollChange(_vertScrollView, scrollX, scrollY, oldScrollX, scrollY);
    }

    private class VerticalScrollChangeListener implements View.OnScrollChangeListener
    {
        VerticalScrollChangeListener( NativeScrollViewManager man )
        {
            _man = man;
        }

        public void onScrollChange (View view,
                                    int scrollX,
                                    int scrollY,
                                    int oldScrollX,
                                    int oldScrollY)
        {
            _man.vertScrollChange(scrollY, oldScrollY);
        }

        private NativeScrollViewManager _man;
    }

    private class HorizontalScrollChangeListener implements View.OnScrollChangeListener
    {
        HorizontalScrollChangeListener( NativeScrollViewManager man )
        {
            _man = man;
        }

        public void onScrollChange (View view,
                                    int scrollX,
                                    int scrollY,
                                    int oldScrollX,
                                    int oldScrollY)
        {
            _man.horzScrollChange(scrollX, oldScrollX);
        }

        private NativeScrollViewManager _man;
    }



    private class VerticalSubScrollView extends ScrollView
    {
        public VerticalSubScrollView(Context context)
        {
            super(context);
        }

        public VerticalSubScrollView(Context context, AttributeSet attrs)
        {
            super(context, attrs);
        }

        public VerticalSubScrollView(Context context, AttributeSet attrs, int defStyleAttr)
        {
            super(context, attrs, defStyleAttr);
        }


        protected void onLayout (boolean changed,
                       int l,
                       int t,
                       int r,
                       int b)
        {
            int count = getChildCount();
            if(count>0)
            {
                View child = getChildAt(0);

                // the child view is the horizontal scroll view.
                // Make it the same width as us, and set its height to itsheight
                // desired height
                child.layout(
                        0,
                        0,
                        r-l,
                        child.getMeasuredHeight() );
            }
        }
    }

    private VerticalSubScrollView   _vertScrollView;
    private HorizontalScrollView    _horzScrollView;
    private NativeViewGroup         _contentParent;
};
