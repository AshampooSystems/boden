#ifndef BDN_ANDROID_ButtonCore_H_
#define BDN_ANDROID_ButtonCore_H_

#include <bdn/android/ViewCore.h>
#include <bdn/android/JButton.h>
#include <bdn/IButtonCore.h>
#include <bdn/Button.h>


namespace bdn
{
namespace android
{


class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
{
public:
    ButtonCore( Button* pOuterButton )
     : ViewCore( pOuterButton, newObj<JButton>() )
    {
        _pJButton = cast<JButton>( getJView() );

        setLabel( pOuterButton->label() );
    }

    P<JButton> getJButton()
    {
        return _pJButton;
    }


    void setLabel(const String& label) override
    {
        _pJButton->setText( label );

        getOuterView()->needSizingInfoUpdate();
    }

    void clicked() override
    {
        ClickEvent evt( getOuterView() );
            
        cast<Button>(getOuterView())->onClick().notify(evt);
    }

private:
    P<JButton> _pJButton;
};


}    
}


#endif


