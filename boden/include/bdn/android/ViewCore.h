#ifndef BDN_ANDROID_ViewCore_H_
#define BDN_ANDROID_ViewCore_H_


namespace bdn
{
namespace android
{

class ViewCore;

}
}

#include <bdn/IViewCore.h>

#include <bdn/java/NativeWeakPointer.h>

#include <bdn/android/JNativeViewCoreClickListener.h>
#include <bdn/android/JView.h>
#include <bdn/android/JNativeViewGroup.h>
#include <bdn/android/UIProvider.h>

#include <bdn/log.h>


namespace bdn
{
namespace android
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
    ViewCore(View* pOuterView, JView* pJView, bool initBounds=true )
    {
        _pJView = pJView;
        _outerViewWeak = pOuterView;

        _uiScaleFactor = 1;

        // set a weak pointer to ourselves as the tag object of the java view
        _pJView->setTag( bdn::java::NativeWeakPointer(this) );

        _defaultPadding = Margin( _pJView->getPaddingTop(),
                                  _pJView->getPaddingRight(),
                                  _pJView->getPaddingBottom(),
                                  _pJView->getPaddingLeft() );

        setVisible( pOuterView->visible() );

        _addToParent( pOuterView->getParentView() );

        setPadding( pOuterView->padding() );

        if(initBounds)
            setBounds( pOuterView->bounds() );

        // initialize the onClick listener. It will call the view core's
        // virtual clicked() method.
        bdn::android::JNativeViewCoreClickListener listener;
        _pJView->setOnClickListener( listener );
    }

    ~ViewCore()
    {
        if(_pJView!=nullptr)
        {
            // remove the the reference to ourselves from the java-side view object.
            // Note that we hold a strong reference to the java-side object,
            // So we know that the reference to the java-side object is still valid.
            _pJView->setTag(bdn::java::JObject(bdn::java::Reference()));

            _pJView = nullptr;
        }
    }

    static ViewCore* getViewCoreFromJavaViewRef( const bdn::java::Reference& javaViewRef )
    {
        if(javaViewRef.isNull())
            return nullptr;
        else
        {
            JView view(javaViewRef);
            bdn::java::NativeWeakPointer viewTag( view.getTag().getRef_() );

            return static_cast<ViewCore*>( viewTag.getPointer() );
        }
    }


    /** Returns a pointer to the outer View object, if this core is still attached to it
        or null otherwise.*/
    P<const View> getOuterViewIfStillAttached() const
    {
        return _outerViewWeak.toStrong();
    }
    
    P<View> getOuterViewIfStillAttached()
    {
        return _outerViewWeak.toStrong();
    }
    

    /** Returns a pointer to the accessor object for the java-side view object.
     */
    JView& getJView()
    {
        return *_pJView;
    }


    
    void setVisible(const bool& visible) override
    {
        _pJView->setVisibility(visible ? JView::Visibility::visible : JView::Visibility::invisible );
    }
        
    void setPadding(const Nullable<UiMargin>& padding) override
    {
        Margin pixelPadding;
        if(padding.isNull())
            pixelPadding = _defaultPadding;
        else
            pixelPadding = uiMarginToPixelMargin(padding);

        _pJView->setPadding(pixelPadding.left, pixelPadding.top, pixelPadding.right,
                                pixelPadding.bottom);
    }


    /** Returns the view core associated with this view's parent view.
     *  Returns null if there is no parent view or if the parent does not
     *  have a view core associated with it.*/
    ViewCore* getParentViewCore()
    {
        return getViewCoreFromJavaViewRef( _pJView->getParent().getRef_() );
    }


    void setBounds(const Rect& bounds) override
    {
        //logInfo("Settings size to "+std::to_string(bounds.width)+"x"+std::to_string(bounds.height));

        bdn::java::JObject parent( _pJView->getParent() );

        if(parent.isNull_())
        {
            // we do not have a parent => we cannot set any bounds.
            // Simply do nothing.
        }
        else
        {
            // the parent of all our views is ALWAYS a NativeViewGroup object.
            JNativeViewGroup parentView( parent.getRef_() );

            parentView.setChildBounds( getJView(), bounds.x, bounds.y, bounds.width, bounds.height );
        }
    }



    int uiLengthToPixels(const UiLength& uiLength) const override;
    
    
    Margin uiMarginToPixelMargin(const UiMargin& margin) const override
    {
        return Margin(
                uiLengthToPixels(margin.top),
                uiLengthToPixels(margin.right),
                uiLengthToPixels(margin.bottom),
                uiLengthToPixels(margin.left) );
    }
    

    
    Size calcPreferredSize(int availableWidth=-1, int availableHeight=-1) const override
    {
		int widthSpec;
        int heightSpec;

        if(availableWidth<0 || !canAdjustWidthToAvailableSpace())
            widthSpec = JView::MeasureSpec::makeMeasureSpec(0, JView::MeasureSpec::unspecified);
        else
            widthSpec = JView::MeasureSpec::makeMeasureSpec(availableWidth, JView::MeasureSpec::atMost);

        if(availableHeight<0 || !canAdjustHeightToAvailableSpace())
            heightSpec = JView::MeasureSpec::makeMeasureSpec(0, JView::MeasureSpec::unspecified);
        else
            heightSpec = JView::MeasureSpec::makeMeasureSpec(availableHeight, JView::MeasureSpec::atMost);

        _pJView->measure( widthSpec, heightSpec );

        int width = _pJView->getMeasuredWidth();
        int height = _pJView->getMeasuredHeight();

        //logInfo("Preferred size of "+std::to_string((int64_t)this)+" "+String(typeid(*this).name())+" : ("+std::to_string(width)+"x"+std::to_string(height)+"); available: ("+std::to_string(availableWidth)+"x"+std::to_string(availableHeight)+") ");

        return Size(width, height);
	}
    
    
    bool tryChangeParentView(View* pNewParent) override
    {
        _addToParent(pNewParent);
        
        return true;
    }


    /** Called when the user clicks on the view.
     *
     *  The default implementation does nothing.
     * */
    virtual void clicked()
    {
    }



    /** Returns the current UI scale factor. This depends on the pixel density
     *  of the current display. On high DPI displays the scale factor is >1 to scale
     *  up UI elements to use more physical pixels.
     *
     *  Scale factor 1 means a "medium" DPI setting of 160 dpi.
     *
     *  Note that the scale factor can also be <1 if the display has a very low dpi (lower than 160).
     *
     *  Also note that the scale factor can change at runtime if the view switches to another display.
     *
     **/
    double getUiScaleFactor() const
    {
        return _uiScaleFactor;
    }


    /** Changes the UI scale factor of this view and all its child views.
     *  See getUiScaleFactor()
     * */
    void setUiScaleFactor(double scaleFactor)
    {
        if(scaleFactor!=_uiScaleFactor)
        {
            _uiScaleFactor = scaleFactor;

            P<View> pView = getOuterViewIfStillAttached();
            std::list<P<View> > childList;
            if(pView!=nullptr)
                pView->getChildViews(childList);

            for (P<View> &pChild: childList) {
                P<ViewCore> pChildCore = cast<ViewCore>(pChild->getViewCore());

                if (pChildCore != nullptr)
                    pChildCore->setUiScaleFactor(scaleFactor);
            }
        }
    }


protected:
    /** Returns true if the view can adjust its width to fit into
		a certain size of available space.

		If this returns false then calcPreferredSize will ignore the
		availableWidth parameter.

		The default implementation returns false.
	*/
    virtual bool canAdjustWidthToAvailableSpace() const
    {
        return false;
    }

    /** Returns true if the view can adjust its height to fit into
        a certain size of available space.

        If this returns false then calcPreferredSize will ignore the
        availableHeight parameter.

        The default implementation returns false.
    */
    virtual bool canAdjustHeightToAvailableSpace() const
    {
        return false;
    }


private:
    void _addToParent(View* pParent)
    {
        if(pParent!=nullptr)
        {
            P<ViewCore> pParentCore = cast<ViewCore>(pParent->getViewCore());
            if(pParentCore==nullptr)
                throw ProgrammingError("Internal error: parent of bdn::android::ViewCore does not have a core.");

            JNativeViewGroup parentGroup( pParentCore->getJView().getRef_() );

            parentGroup.addView( *_pJView );

            setUiScaleFactor( pParentCore->getUiScaleFactor() );
        }
    }

    WeakP<View>     _outerViewWeak;

private:
    P<JView>        _pJView;
    double          _uiScaleFactor;

    Margin          _defaultPadding;
};

}
}

#endif


