#ifndef BDN_ANDROID_TextViewCore_H_
#define BDN_ANDROID_TextViewCore_H_

#include <bdn/android/ViewCore.h>
#include <bdn/android/JTextView.h>
#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>


namespace bdn
{
namespace android
{


class TextViewCore : public ViewCore, BDN_IMPLEMENTS ITextViewCore
{
private:
    static P<JTextView> _createJTextView(TextView* pOuter)
    {
        // we need to know the context to create the view.
        // If we have a parent then we can get that from the parent's core.
        P<View> 	pParent = pOuter->getParentView();
        if(pParent==nullptr)
            throw ProgrammingError("TextViewCore instance requested for a TextView that does not have a parent.");

        P<ViewCore> pParentCore = cast<ViewCore>( pParent->getViewCore() );
        if(pParentCore==nullptr)
            throw ProgrammingError("TextViewCore instance requested for a TextView with core-less parent.");

        JContext context = pParentCore->getJView().getContext();

        P<JTextView> pTextView = newObj<JTextView>(context);

        pTextView->setHorizontallyScrolling(false);
        pTextView->setSingleLine(false);
        pTextView->setMaxLines(100);
        pTextView->setBreakStrategy( JTextView::BreakStrategy::simple );

        return pTextView;
    }

public:
    TextViewCore( TextView* pOuterTextView )
     : ViewCore( pOuterTextView, _createJTextView(pOuterTextView) )
    {
        _pJTextView = cast<JTextView>( &getJView() );

        setText( pOuterTextView->text() );
    }


    JTextView& getJTextView()
    {
        return *_pJTextView;
    }


    void setText(const String& text) override
    {
        // Android treats carriage return like a space. So we filter those out.
        String textToSet = text;
        textToSet.findReplace("\r", "");

        _pJTextView->setText( textToSet );

        // we must re-layout the button - otherwise its preferred size is not updated.
        _pJTextView->requestLayout();
    }

    void setBounds(const Rect& bounds) override
    {
        ViewCore::setBounds(bounds);

        // for some reason the TextView does not wrap its text, unless we explicitly set the
        // width with setMaxWidth (even if the widget's size is actually smaller than the text).
        // This seems to be a bug in android.
        _pJTextView->setMaxWidth( bounds.width );
        _currWidth = bounds.width;
    }

    Size calcPreferredSize(int availableWidth=-1, int availableHeight=-1) const override
    {
        // we must unset the fixed width we set in the last setBounds call, otherwise it will influence
        // the size we measure here.

        if(_currWidth!=0x7fffffff && _pJTextView!=nullptr)
            _pJTextView->setMaxWidth(0x7fffffff);

        Size resultSize;

        try
        {
            resultSize = ViewCore::calcPreferredSize(availableWidth, availableHeight);
        }
        catch(...)
        {
            try
            {
                if(_currWidth!=0x7fffffff && _pJTextView!=nullptr)
                    _pJTextView->setMaxWidth(_currWidth);
            }
            catch(...)
            {
                // ignore.
            }

            throw;
        }

        if(_currWidth!=0x7fffffff && _pJTextView!=nullptr)
            _pJTextView->setMaxWidth(_currWidth);

        return resultSize;
    }

protected:
    bool canAdjustWidthToAvailableSpace() const override
    {
        return true;
    }

private:
    P<JTextView> _pJTextView;

    int          _currWidth=0x7fffffff;
};


}    
}


#endif


