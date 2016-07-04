#include <bdn/init.h>

#import <Cocoa/Cocoa.h>

#import <bdn/mac/ButtonCore.hh>


@interface BdnButtonClickManager : NSObject

    @property bdn::mac::ButtonCore* pButtonCore;

@end


@implementation BdnButtonClickManager

    -(void)setButtonCore:(bdn::mac::ButtonCore*)pCore
    {
        _pButtonCore = pCore;
    }

    -(void)clicked
    {
        _pButtonCore->generateClick();
    }

@end



namespace bdn
{
namespace mac
{

ButtonCore::ButtonCore(Button* pOuterButton)
    : ButtonCoreBase( pOuterButton, _createNsButton() )
{
    BdnButtonClickManager* clickMan = [BdnButtonClickManager alloc];
    
    [clickMan setButtonCore:this];
    
    _clickManager = clickMan;
    
    setLabel( pOuterButton->label() );
    
    
    [_nsButton setTarget:clickMan];
    [_nsButton setAction:@selector(clicked)];
}


void ButtonCore::generateClick()
{
    P<Button> pOuterButton = cast<Button>(getOuterView());
    
    bdn::ClickEvent evt(pOuterButton);
    
    pOuterButton->onClick().notify(evt);
}


}
}
