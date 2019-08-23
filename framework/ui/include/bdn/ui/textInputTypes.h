#pragma once

namespace bdn::ui
{
    enum class AutocorrectionType
    {
        Default,
        No,
        Yes
    };

    // Note: Flutter also has a custom type "newline" doing some magic to make multiline text field
    // software keyboard behavior easier to use:
    // https://api.flutter.dev/flutter/services/TextInputAction-class.html#constants
    enum class ReturnKeyType
    {
        Default,       // Android: IME_ACTION_UNSPECIFIED, iOS: UIReturnKeyDefault
        Continue,      // Android: N/A, iOS: UIReturnKeyContinue
        Done,          // Android: IME_ACTION_DONE, iOS: UIReturnKeyDone
        EmergencyCall, // Android: N/A, iOS: UIReturnKeyEmergencyCall
        Go,            // Android: IME_ACTION_GO, iOS: UIReturnKeyGo
        Join,          // Android: N/A, iOS: UIReturnKeyJoin
        Next,          // Android: IME_ACTION_NEXT, iOS: UIReturnKeyNext
        None,          // Android: IME_ACTION_NONE, iOS: N/A
        Previous,      // Android: IME_ACTION_PREVIOUS, iOS: N/A
        Route,         // Android: N/A, iOS: UIReturnKeyRoute
        Search,        // Android: IME_ACTION_SEARCH, iOS: UIReturnKeySearch
        Send           // Android: IME_ACTION_SEND, iOS: UIReturnKeySend
    };

    enum class TextInputType
    {
        Text,
        URL,
        Number,
        Phone,
        EMail,
        DateTime,
        MultiLine,
    };
}
