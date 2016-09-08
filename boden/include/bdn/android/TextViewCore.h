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

        return newObj<JTextView>(context);
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
        _pJTextView->setText( text );

        // we must re-layout the button - otherwise its preferred size is not updated.
        _pJTextView->requestLayout();
    }

private:
    P<JTextView> _pJTextView;
};


}    
}


#endif


