#ifndef BDN_Checkbox_H_
#define BDN_Checkbox_H_

#include <bdn/constants.h>
#include <bdn/View.h>
#include <bdn/ClickEvent.h>
#include <bdn/ICheckboxCore.h>
#include <bdn/SimpleNotifier.h>

namespace bdn
{

/** A control providing the user with an on/off choice.

    The Checkbox class implements a checkbox control. If the target platform does not
    provide a native checkbox widget, a platform-specific substitution provided by the
    framework will be displayed instead.

    Use checkboxes for selecting options in settings dialogs, property panels, or forms.
    Checkboxes are common on Desktop platforms, but they rarely used on mobile platforms.

    If your use case does neither require mixed state nor additional user confirmation
    for the setting to take effect, consider using the Toggle class instead. Toggles 
    automatically adapt their appearence with regard to the target platform.

    Checkboxes can be used as a dual-state (on/off) or tri-state (on/off/mixed)
    toggle. Mixed state (sometimes also referred to as indeterminate or inconsistent
    state) is used to represent a partial state, usually summarizing the state of
    other checkbox items in a group of which only some items are checked.

    While you can set a Checkbox to mixed state programatically, user interaction
    is limited to setting the on or off state. When the user clicks a Checkbox which
    is currently set to mixed state, the state transition behavior is defined by
    the platform: on Win32, Linux, macOS, iOS, and on the web, clicking on a mixed
    state checkbox yields the `on` state while on Windows UWP it yields the `off` state.

    Note: Both Android and some mobile web browsers do not support mixed state checkboxes.
    On these systems, Checkbox instances can still be set to mixed state, but the UI will
    display them as off.
 */
class Checkbox : public View
{
public:
    Checkbox()
    {
        _pOnClick = newObj< SimpleNotifier<const ClickEvent&> >();
    }    
    
    
    /** The checkbox's label */
    BDN_VIEW_PROPERTY( String, label, setLabel, ICheckboxCore, influencesPreferredSize() );
    
    /** State of the checkbox, see TriState */
    BDN_VIEW_PROPERTY( TriState, state, setState, ICheckboxCore, influencesNothing() );
    
    
    /** A notifier for click events. Subscribe to this notifier if you want to be notified about
        click events. Click events are posted when the user clicks on the checkbox or checkbox label.
     */
    ISyncNotifier<const ClickEvent&>& onClick()
    {
        return *_pOnClick;
    }    

    
    /** Returns the name of Checkbox core objects */  
    static String getCheckboxCoreTypeName()
    {
        return "bdn.CheckboxCore";
    }

    /** Returns the core type name */
    String getCoreTypeName() const override
    {
        return getCheckboxCoreTypeName();
    }

protected:
    P< SimpleNotifier<const ClickEvent&> > _pOnClick;
};

}
    
#endif // BDN_Checkbox_H_
