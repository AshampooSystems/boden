#ifndef BDN_WINUWP_UwpLayoutBridge_H_
#define BDN_WINUWP_UwpLayoutBridge_H_

#include <bdn/winuwp/IUwpLayoutDelegate.h>


namespace bdn
{
namespace winuwp
{


/** Manages the connection between the UWP layout system and the layout
    system of the Boden framework.

    There is a global instance of the bridge that manages the interactions with the UWP
    layout system (see UwpLayoutBridge::get() ).
    
    Whenever a UWP layout operation (Measure or Arrange call)
    happens for a Boden container view then that call must be passed along to
    the bridge.
    
    The bridge takes care of executing the finalizer function
    at the end of the Measure phase (see IUwpLayoutDelegate::finalizeUwpMeasure),

    The bridge also tracks when layout requests have been issued and provides methods
    to check if a layout operation is still pending or not.

    This class is NOT thread safe and should only be used from the main thread.

    */
class UwpLayoutBridge : public Base
{
public:
    UwpLayoutBridge();


    /** Returns the global bridge instance.*/
    static UwpLayoutBridge& get();
    

    /** Performs a measure call on the specified layout delegate (calls
        IUwpLayoutDelegate::uwpMeasureOverride() ).

        availableSpace is the parameter for uwpMeasureOverride.
        
        The return value is the return value of uwpMeasureOverride.
    
        This function automatically tracks recursive measure calls
        and ensures that IUwpLayoutDelegate::finalizeUwpMeasure
        is called when the top level measure call finishes (i.e. at the
        end of the Measure phase).

        doMeasure also ensures that the "pending" flags (see isMeasurePending() )
        are updated as necessary.

    */
    Size doMeasure( IUwpLayoutDelegate* pDelegate, const Size& availableSpace );


    /** Performs an arrange call on the specified layout delegate (calls
        IUwpLayoutDelegate::uwpArrangeOverride() ).
    
        finalSize is the parameter for uwpArrangeOverride.
        
        The return value is the return value of uwpArrangeOverride.

        doArrange also ensures that the "pending" flags (see isArrangePending() )
        are updated as necessary.

    */
    Size doArrange( IUwpLayoutDelegate* pDelegate, const Size& finalSize );




    /** Returns true if a measure call was scheduled and is still pending.*/
    bool isMeasurePending() const;


    /** Returns true if an arrange call was scheduled and is still pending.*/
    bool isArrangePending() const;

    
    /** Invalidates the measure information of the specified UWP view.
    
        Use this instead of calling InvalidateMeasure directly to ensure that
        the bridge can track pending layout requests correctly.
        */
    void invalidateMeasure(::Windows::UI::Xaml::FrameworkElement^ pElement);


    /** Invalidates the arrange information of the specified UWP view.
    
        Use this instead of calling InvalidateArrange directly to ensure that
        the bridge can track pending layout requests correctly.
        */
    void invalidateArrange(::Windows::UI::Xaml::FrameworkElement^ pElement);

    
    /** This must be called in response to the global "LayoutUpdated" event when any layout
        operation in the view tree has taken place.
        WindowCore automatically performs this call, so it is usually not necessary to do it
        manually.
        */
    void layoutUpdated();
    

private:
    void measureStarting();
    void measureEnding( IUwpLayoutDelegate* pDelegate, const Size& availableSpace );

    void arrangeStarting();
    void arrangeEnding();

    int  _activeMeasureCalls = 0;

    bool _arrangePending = false;
    bool _measurePending = false;    
};


}
}

#endif
