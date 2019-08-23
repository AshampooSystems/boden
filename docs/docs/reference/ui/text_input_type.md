path: tree/master/framework/ui/include/bdn/ui
source: textInputTypes.h

# TextInputType

The type of input which a text entry view and its keyboard should optimize for.

## Definition

```C++
namespace bdn::ui
{
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
```

## Enum Values

* **TextInputType::Text**

	Use case: A single line of text

	Corresponds to `UIKeyboardTypeDefault` on iOS and to `InputType::TYPE_CLASS_TEXT` on Android.

* **TextInputType::URL**

	Use case: A URL

	Corresponds to `UIKeyboardTypeURL` on iOS and to `InputType::TYPE_CLASS_TEXT | InputType::TYPE_TEXT_VARIATION_URI` on Android.

* **TextInputType::Number**

	Use case: A single number with possible decimal and sign

	Corresponds to `UIKeyboardTypeNumbersAndPunctuation` on iOS and to `InputType::TYPE_CLASS_NUMBER | InputType::TYPE_NUMBER_FLAG_DECIMAL | InputType::TYPE_NUMBER_FLAG_SIGNED` on Android.

* **TextInputType::Phone**

	Use case: A phone number

	Corresponds to `UIKeyboardTypePhonePad`  on iOS and to `InputType::TYPE_CLASS_PHONE` on Android.

* **TextInputType::EMail**

	Use case: An email address

	Corresponds to `UIKeyboardTypeEmailAddress`  on iOS and to `InputType::TYPE_CLASS_TEXT | InputType::TYPE_TEXT_VARIATION_EMAIL_ADDRESS` on Android.

* **TextInputType::DateTime**

	Use case: A date and time value

	Corresponds to `UITextContentTypeDefault` on iOS and to `InputType::TYPE_CLASS_DATETIME` on Android.

* **TextInputType::MultiLine**

	Use case: A multiline text input

	Corresponds to `UITextContentTypeDefault`  on iOS and to `InputType::TYPE_CLASS_TEXT | InputType::TYPE_TEXT_VARIATION_LONG_MESSAGE` on Android.
