#ifndef BDN_ANDROID_ViewCore_H_
#define BDN_ANDROID_ViewCore_H_

#include <bdn/IViewCore.h>

#include <bdn/java/NativeWeakPointer.h>

#include <bdn/android/JNativeViewCoreClickListener.h>
#include <bdn/android/JView.h>
#include <bdn/android/JNativeViewGroup.h>
#include <bdn/android/UIProvider.h>

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

        // set a weak pointer to ourselves as the tag object of the java view
        _pJView->setTag( bdn::java::NativeWeakPointer(this) );

        setVisible( pOuterView->visible() );
        setPadding( pOuterView->padding() );

        _addToParent( pOuterView->getParentView() );

        if(initBounds)
            setBounds( pOuterView->bounds() );

        // initialize the onClick listener. It will call the view core's
        // virtual clicked() method.
        bdn::android::JNativeViewCoreClickListener listener;
        _pJView->setOnClickListener( listener );
    }

    ~ViewCore()
    {
        // remove the the reference to ourselves from the java-side view object.
        // Note that we hold a strong reference to the java-side object,
        // So we know that the reference to the java-side object is still valid.
        _pJView->setTag( bdn::java::JObject( bdn::java::Reference() ) );
    }


    static ViewCore* getViewCoreFromJavaView( const bdn::java::Reference& javaViewRef )
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


    const View* getOuterView() const
    {
        return _pOuterViewWeak;
    }
    
    View* getOuterView()
    {
        return _pOuterViewWeak;
    }
    

    /** Returns a pointer to the accessor object for the java-side view object.
     */
    JView* getJView()
    {
        return _pJView;
    }


    
    void setVisible(const bool& visible) override
    {
        _pJView->setVisibility(visible ? JView::Visibility::visible : JView::Visibility::invisible );
    }

    void setMargin(const UiMargin& margin) override
    {
        // we don't care about OUR margin. Our parent uses it during layout.
        // So, do nothing here.
    }
    
    void setPadding(const UiMargin& padding) override
    {
        Margin pixelPadding = uiMarginToPixelMargin(padding);

        _pJView->setPadding( pixelPadding.left, pixelPadding.top, pixelPadding.right, pixelPadding.bottom );

        _pOuterViewWeak->needSizingInfoUpdate();
    }


    /** Returns the view core associated with this view's parent view.
     *  Returns null if there is no parent view or if the parent does not
     *  have a view core associated with it.*/
    ViewCore* getParentViewCore()
    {
        return getViewCoreFromJavaView( _pJView->getParent().getRef_() );
    }


    void setBounds(const Rect& bounds) override
    {
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

            parentView.setChildBounds( *getJView(), bounds.x, bounds.y, bounds.width, bounds.height );
        }
    }
    
    
    int uiLengthToPixels(const UiLength& uiLength) const override
    {
        return UiProvider::get().uiLengthToPixels(uiLength);
    }
    
    
    Margin uiMarginToPixelMargin(const UiMargin& margin) const override
    {
        return UiProvider::get().uiMarginToPixelMargin(margin);
    }
    
    
    
    
    Size calcPreferredSize() const override
    {
        return _calcPreferredSize(-1, -1);
    }
    
    
    int calcPreferredHeightForWidth(int width) const override
    {
        return _calcPreferredSize(width, -1).height;
    }
    
    
    int calcPreferredWidthForHeight(int height) const override
    {
        return _calcPreferredSize(-1, height).width;
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

private:
    Size _calcPreferredSize(int forWidth, int forHeight) const
    {
        int widthSpec;
        int heightSpec;

        if(forWidth<0)
            widthSpec = JView::MeasureSpec::makeMeasureSpec(0, JView::MeasureSpec::unspecified);
        else
            widthSpec = JView::MeasureSpec::makeMeasureSpec(forWidth, JView::MeasureSpec::atMost);

        if(forHeight<0)
            heightSpec = JView::MeasureSpec::makeMeasureSpec(0, JView::MeasureSpec::unspecified);
        else
            heightSpec = JView::MeasureSpec::makeMeasureSpec(forHeight, JView::MeasureSpec::atMost);

        _pJView->measure( widthSpec, heightSpec );

        int width = _pJavaView->getMeasuredWidth();
        int height = _pJavaView->getMeasuredHeight();

        return Size(width, height);
    }

    void _addToParent(View* pParent)
    {
        if(pParent!=nullptr)
        {
            P<ViewCore> pParentCore = cast<ViewCore>( pParent->getViewCore() );

            P<ViewCore> pParentCore = cast<ViewCore>(pParent->getViewCore());
            if(pParentCore==nullptr)
                throw ProgrammingError("Internal error: parent of bdn::android::ViewCore does not have a core.");

            pParentCore->addChild( this );
        }
    }

    View*           _pOuterViewWeak;
    P<JView>        _pJView;
};

}
}

#endif


