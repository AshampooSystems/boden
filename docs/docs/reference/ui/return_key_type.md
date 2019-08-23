path: tree/master/framework/ui/include/bdn/ui
source: textInputTypes.h

# ReturnKeyType

The type of return key to use when displaying a software keyboard.

## Definition

```C++
namespace bdn::ui
{
	enum class ReturnKeyType
	{
		Default,		// Android: IME_ACTION_UNSPECIFIED, iOS: UIReturnKeyDefault
		Continue,       // Android: N/A, iOS: UIReturnKeyContinue
		Done,           // Android: IME_ACTION_DONE, iOS: UIReturnKeyDone
		EmergencyCall,  // Android: N/A, iOS: UIReturnKeyEmergencyCall
		Go,             // Android: IME_ACTION_GO, iOS: UIReturnKeyGo
		Join,           // Android: N/A, iOS: UIReturnKeyJoin
		Next,           // Android: IME_ACTION_NEXT, iOS: UIReturnKeyNext
		None,           // Android: IME_ACTION_NONE, iOS: N/A
		Previous,       // Android: IME_ACTION_PREVIOUS, iOS: N/A
		Route,          // Android: N/A, iOS: UIReturnKeyRoute
		Search,         // Android: IME_ACTION_SEARCH, iOS: UIReturnKeySearch
		Send            // Android: IME_ACTION_SEND, iOS: UIReturnKeySend
	};	
}
```

## Enum Values

* **ReturnKeyType::Default**

	Use case: Let the operating system decide what's the most appropriate title for the return key button.

	Corresponds to `UIReturnKeyDefault` on iOS and to `IME_ACTION_UNSPECIFIED` on Android. Shows "return" in the return key button on iOS. Most likely shows "return" or "done" in the return key button on Android.

* **ReturnKeyType::Continue**

	Use case: Continue to the next page when entering text while navigating.

	Corresponds to `UIReturnKeyContinue` on iOS. Not supported on Android (falls back to `IME_ACTION_UNSPECIFIED`). Shows "Continue" in the return key button on iOS.

* **ReturnKeyType::Done**

	Use case: Complete a form or other group of inputs.

	Corresponds to `UIReturnKeyDone` on iOS and to `IME_ACTION_DONE` on Android. Shows "Done" as the return key button's title.

* **ReturnKeyType::EmergencyCall**

	Use case: Call emergency services.

	Corresponds to `UIReturnKeyEmergencyCall` on iOS. Not supported on Android (falls back to `IME_ACTION_UNSPECIFIED`). Shows "Emergency Call" as the return key button's title.

* **ReturnKeyType::Go**

	Use case: Go to a given destination entered in a form field.

	Corresponds to `UIReturnKeyTypeGo` on iOS and to `IME_ACTION_GO` on Android. The title displayed in the return key button is "Go".

* **ReturnKeyType::Join**

	Use case: Join something, e.g. a wireless network.

	Corresponds to `UIReturnKeyTypeJoin` on iOS. Not supported on Android (falls back to `IME_ACTION_UNSPECIFIED`). The return key button displays "Join".

* **ReturnKeyType::Next**

	Use case: Move to the next input source.

	Corresponds to `UIReturnKeyTypeNext` on iOS and to `IME_ACTION_NEXT` on Android. The return key button displays "Next".

* **ReturnKeyType::None**

	Use case: It doesn't make sense to display a return key for the given input.

	Unsupported on iOS. Corresponds to `IME_ACTION_NONE` on Android. The software keyboard will most likely display a return key.

* **ReturnKeyType::Previous**

	Use case: Return to the previous input source, e.g. in a form or other group of input sources.

	Unsupported on iOS. Corresponds to `IME_ACTION_PREVIOUS` on Android. The software keyboard will display a return button indicating backwards movement, e.g. a left-facing arrow.

* **ReturnKeyType::Route**

	Use case: Provide driving directions or other route information.

	Corresponds to `UIReturnKeyTypeRoute` on iOS. Not supported on Android. Displays "Route" in the return key button.

* **ReturnKeyType::Search**

	Use case: Search for something.

	Corresponds to `UIReturnKeySearch` on iOS and to `IME_ACTION_SEARCH` on Android. Shows "Search" as the return key button's title on iOS and displays a magnifying glass icon on Android.

* **ReturnKeyType::Send**

	Use case: Send something, e.g. a message.

	Corresponds to `UIReturnKeySend` on iOS and to `IME_ACTION_SEND` on Android. Shows "Send" as the return key button's title on iOS and displays an icon representing a "send" action (e.g. a paper plane) on Android.

