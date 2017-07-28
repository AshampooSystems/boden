#include <bdn/init.h>
#include <bdn/winuwp/UwpLayoutBridge.h>

#include <bdn/winuwp/platformError.h>


namespace bdn
{
namespace winuwp
{


UwpLayoutBridge& UwpLayoutBridge::get()
{
    static UwpLayoutBridge globalInstance;

    return globalInstance;
}


UwpLayoutBridge::UwpLayoutBridge()
{
}

void UwpLayoutBridge::invalidateArrange(::Windows::UI::Xaml::FrameworkElement^ pElement)
{
    BDN_WINUWP_TO_STDEXC_BEGIN;

    try
    {
		pElement->InvalidateArrange();
        _arrangePending = true;
    }
    catch(::Platform::DisconnectedException^ e)
    {
        // view was already destroyed. Ignore this.
    }

    BDN_WINUWP_TO_STDEXC_END;
}


void UwpLayoutBridge::invalidateMeasure(::Windows::UI::Xaml::FrameworkElement^ pElement)
{
    OutputDebugString( ( "InvalidateMeasure on "+String( pElement->GetType()->ToString()->Data() )+"\n").asWidePtr() );

    BDN_WINUWP_TO_STDEXC_BEGIN;

    try
    {        
		pElement->InvalidateMeasure();        
        _measurePending = true;
    }
    catch(::Platform::DisconnectedException^ e)
    {
        // view was already destroyed. Ignore this.
    }

    BDN_WINUWP_TO_STDEXC_END;
}

void UwpLayoutBridge::measureStarting()
{
    // we assume that when one measure call starts that ALL pending
    // measure calls will immediately follow. So we reset the pending flag.
    _measurePending = false;

    _activeMeasureCalls++;
}

void UwpLayoutBridge::measureEnding(IUwpLayoutDelegate* pDelegate, const Size& availableSpace)
{
    // we also assume that arrange operations that were scheduled DURING
    // another arrange will be performed immediately. So we also reset the
    // pending flag again when arrange ends.
    _measurePending = false;

    _activeMeasureCalls--;

    if(_activeMeasureCalls==0)
    {
        // the measure phase ends. So we must finalize the UWP measure.

        // measure calls that occur during finalize should not trigger another
        // finalization. We set _activeFinalizers to a dummy number to prevent that.
        _activeMeasureCalls = 0x10000000;

        OutputDebugString( (String(typeid(*pDelegate).name()) +" finalizeUwpMeasure("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+")\n").asWidePtr() );

        try
        {
			pDelegate->finalizeUwpMeasure( availableSpace );
        }
        catch(...)
        {
            _activeMeasureCalls = 0;
            throw;
        }

        OutputDebugString( ("/"+String(typeid(*pDelegate).name()) +" finalizeUwpMeasure()\n").asWidePtr() );

        _activeMeasureCalls = 0;
    }
}

Size UwpLayoutBridge::doMeasure( IUwpLayoutDelegate* pDelegate, const Size& availableSpace )
{
    measureStarting();

    Size resultSize;
    try
    {
        resultSize = pDelegate->uwpMeasureOverride(availableSpace);
    }
    catch(...)
    {
        measureEnding( pDelegate, availableSpace );
        throw;
    }

    measureEnding( pDelegate, availableSpace );

    return resultSize;
}


void UwpLayoutBridge::arrangeStarting()
{
    // we assume that when one arrange call starts that ALL pending
    // arrange calls will immediately follow. So we reset the pending flag.
    _arrangePending = false;
}

void UwpLayoutBridge::arrangeEnding()
{
    // we also assume that arrange operations that were scheduled DURING
    // another arrange will be performed immediately. So we also reset the
    // pending flag again when arrange ends.
    _arrangePending = false;
}

Size UwpLayoutBridge::doArrange( IUwpLayoutDelegate* pDelegate, const Size& finalSize )
{
    arrangeStarting();

    Size resultSize;
    try
    {
        resultSize = pDelegate->uwpArrangeOverride(finalSize);
    }
    catch(...)
    {
        arrangeEnding();
        throw;
    }

    arrangeEnding();       

    return resultSize;
}



bool UwpLayoutBridge::isMeasurePending() const
{
    return _measurePending;
}


bool UwpLayoutBridge::isArrangePending() const
{
    return _arrangePending;
}

void UwpLayoutBridge::layoutUpdated()
{
    _measurePending = false;
    _arrangePending = false;
}


}
}

