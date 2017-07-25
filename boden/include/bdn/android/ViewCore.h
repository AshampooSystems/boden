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
#include <bdn/PixelAligner.h>
#include <bdn/LayoutCoordinator.h>

#include <bdn/java/NativeWeakPointer.h>

#include <bdn/android/JNativeViewCoreClickListener.h>
#include <bdn/android/JView.h>
#include <bdn/android/JNativeViewGroup.h>
#include <bdn/android/UIProvider.h>
#include <bdn/android/IParentViewCore.h>

#include <bdn/log.h>


namespace bdn
{
namespace android
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore, BDN_IMPLEMENTS LayoutCoordinator::IViewCoreExtension
{
public:
    ViewCore(View* pOuterView, JView* pJView)
    {
        _pJView = pJView;
        _outerViewWeak = pOuterView;

        _uiScaleFactor = 1; // will be updated in _addToParent

        // set a weak pointer to ourselves as the tag object of the java view
        _pJView->setTag( bdn::java::NativeWeakPointer(this) );

        setVisible( pOuterView->visible() );

        _addToParent( pOuterView->getParentView() );


        _defaultPixelPadding = Margin( _pJView->getPaddingTop(),
                                  _pJView->getPaddingRight(),
                                  _pJView->getPaddingBottom(),
                                  _pJView->getPaddingLeft() );

        setPadding( pOuterView->padding() );

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
    P<View> getOuterViewIfStillAttached() const
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
            pixelPadding = _defaultPixelPadding;
        else
        {
            Margin dipPadding = uiMarginToDipMargin(padding);

            pixelPadding = Margin(dipPadding.top * _uiScaleFactor,
                                  dipPadding.right * _uiScaleFactor,
                                  dipPadding.bottom * _uiScaleFactor,
                                  dipPadding.left * _uiScaleFactor);
        }

        _pJView->setPadding(pixelPadding.left, pixelPadding.top, pixelPadding.right, pixelPadding.bottom);
    }




    void invalidateSizingInfo(View::InvalidateReason reason) override
    {
        // nothing to do since we do not cache sizing info in the core.
    }


    void needLayout(View::InvalidateReason reason) override;

    void childSizingInfoInvalidated(View* pChild) override;


    void setHorizontalAlignment(const View::HorizontalAlignment& align) override
    {
        // do nothing. The View handles this.
    }

    void setVerticalAlignment(const View::VerticalAlignment& align) override
    {
        // do nothing. The View handles this.
    }


    void setPreferredSizeHint(const Size& hint) override
    {
        // nothing to do by default. Most views do not use this.
    }


    void setPreferredSizeMinimum(const Size& limit) override
    {
        // do nothing. The View handles this.
    }

    void setPreferredSizeMaximum(const Size& limit) override
    {
        // do nothing. The View handles this.
    }






    /** Returns the view core associated with this view's parent view.
     *  Returns null if there is no parent view or if the parent does not
     *  have a view core associated with it.*/
    ViewCore* getParentViewCore()
    {
        return getViewCoreFromJavaViewRef( _pJView->getParent().getRef_() );
    }


    Rect adjustAndSetBounds(const Rect& requestedBounds) override
    {
        Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);

        bdn::java::JObject parent( _pJView->getParent() );

        if(parent.isNull_())
        {
            // we do not have a parent => we cannot set any position.
            // Simply do nothing.
        }
        else
        {
            // the parent of all our views is ALWAYS a NativeViewGroup object.
            JNativeViewGroup parentView( parent.getRef_() );

            parentView.setChildBounds(
                    getJView(),
                    adjustedBounds.x * _uiScaleFactor,
                    adjustedBounds.y * _uiScaleFactor,
                    adjustedBounds.width * _uiScaleFactor,
                    adjustedBounds.height * _uiScaleFactor);
        }

        return adjustedBounds;
    }

    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType) const override
    {
        // align on pixel boundaries
        return PixelAligner(_uiScaleFactor).alignRect( requestedBounds, positionRoundType, sizeRoundType);
    }


    double uiLengthToDips(const UiLength& uiLength) const override;
    
    
    Margin uiMarginToDipMargin(const UiMargin& margin) const override
    {
        return Margin(
                uiLengthToDips(margin.top),
                uiLengthToDips(margin.right),
                uiLengthToDips(margin.bottom),
                uiLengthToDips(margin.left) );
    }
    

    
    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
    {
		int widthSpec;
        int heightSpec;

        if( std::isfinite(availableSpace.width) && canAdjustWidthToAvailableSpace())
        {
            widthSpec = JView::MeasureSpec::makeMeasureSpec(
                    std::lround( stableScaledRoundDown( availableSpace.width, _uiScaleFactor)*_uiScaleFactor ), // round DOWN to the closest pixel then scale up and round to the nearest integer
                    JView::MeasureSpec::atMost);
        }
        else
            widthSpec = JView::MeasureSpec::makeMeasureSpec(0, JView::MeasureSpec::unspecified);


        if( std::isfinite(availableSpace.height) && canAdjustHeightToAvailableSpace())
        {
            heightSpec = JView::MeasureSpec::makeMeasureSpec(
                    std::lround(stableScaledRoundDown( availableSpace.height, _uiScaleFactor) *
                                _uiScaleFactor), // round DOWN to the closest pixel then scale up and round to the nearest integer
                    JView::MeasureSpec::atMost);
        }
        else
            heightSpec = JView::MeasureSpec::makeMeasureSpec(0, JView::MeasureSpec::unspecified);

        _pJView->measure( widthSpec, heightSpec );

        int width = _pJView->getMeasuredWidth();
        int height = _pJView->getMeasuredHeight();

        //logInfo("Preferred size of "+std::to_string((int64_t)this)+" "+String(typeid(*this).name())+" : ("+std::to_string(width)+"x"+std::to_string(height)+"); available: ("+std::to_string(availableWidth)+"x"+std::to_string(availableHeight)+") ");

        // android uses physical pixels. So we must convert to DIPs.
        Size prefSize(width / _uiScaleFactor, height / _uiScaleFactor);

        P<const View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
            prefSize.applyMinimum(pView->preferredSizeMinimum());
            prefSize.applyMaximum(pView->preferredSizeMaximum());
        }

        return prefSize;
	}


    void layout() override
    {
        // do nothing by default. Most views do not have subviews.
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


    virtual double getFontSizeDips() const
    {
        // most views do not have a font attached on android. So for most views we return the
        // sem size.
        return getSemSizeDips();
    }


    double getEmSizeDips() const
    {
        if(_emDipsIfInitialized==-1)
            _emDipsIfInitialized = getFontSizeDips();

        return _emDipsIfInitialized;
    }


    double getSemSizeDips() const;



private:
    void _addToParent(View* pParent)
    {
        if(pParent!=nullptr)
        {
            P<IParentViewCore> pParentCore = cast<IParentViewCore>(pParent->getViewCore());
            if(pParentCore==nullptr)
                throw ProgrammingError("Internal error: parent of bdn::android::ViewCore either does not have a core, or its core does not support child views.");

            pParentCore->addChildJView( *_pJView );

            setUiScaleFactor( pParentCore->getUiScaleFactor() );
        }
    }

    WeakP<View>     _outerViewWeak;

private:
    P<JView>        _pJView;
    double          _uiScaleFactor;

    Margin          _defaultPixelPadding;

    mutable double      _emDipsIfInitialized = -1;
    mutable double      _semDipsIfInitialized = -1;
};

}
}

#endif


