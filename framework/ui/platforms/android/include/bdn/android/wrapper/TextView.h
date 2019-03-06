#pragma once

#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/TextPaint.h>
#include <bdn/android/wrapper/TextWatcher.h>
#include <bdn/android/wrapper/View.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{

    static constexpr char kOnEditorActionListenerName[] = "android/widget/TextView$OnEditorActionListener";
    using OnEditorActionListener = bdn::java::wrapper::JTObject<kOnEditorActionListenerName>;

    constexpr const char kTextViewClassName[] = "android/widget/TextView";

    template <const char *javaClassName = kTextViewClassName, class... ConstructorArguments>
    class BaseTextView : public BaseView<javaClassName, ConstructorArguments...>
    {
      public:
        enum class BreakStrategy
        {
            /** Value for break strategy indicating simple line breaking.
             * Automatic hyphens are not added (though soft hyphens are
             * respected), and modifying text generally doesn't affect the
             * layout before it (which yields a more consistent user
             * experience when editing), but layout may not be the highest
             * quality.*/
            simple = 0,

            /** Value for break strategy indicating high quality line
             * breaking, including automatic hyphenation and doing
             * whole-paragraph optimization of line breaks.*/
            highQuality = 1,

            /** Value for break strategy indicating balanced line breaking.
             * The breaks are chosen
             *  to make all lines as close to the same length as possible,
             * including automatic hyphenation.*/
            balanced = 2
        };

      public:
        using BaseView<javaClassName, ConstructorArguments...>::BaseView;

        JavaMethod<TextPaint()> getPaint{this, "getPaint"};

        JavaMethod<void(java::wrapper::CharSequence)> setText{this, "setText"};
        JavaMethod<java::wrapper::CharSequence()> getText{this, "getText"};

        JavaMethod<void(bool)> setSingleLine{this, "setSingleLine"};

        JavaMethod<void(int)> setBreakStrategy{this, "setBreakStrategy"};

        JavaMethod<void(bool)> setHorizontallyScrolling{this, "setHorizontallyScrolling"};

        JavaMethod<void(int)> setWidth{this, "setWidth"};
        JavaMethod<void(int)> setMaxWidth{this, "setMaxWidth"};
        JavaMethod<void(int)> setMaxHeight{this, "setMaxHeight"};
        JavaMethod<void(int)> setMaxLines{this, "setMaxLines"};

        JavaMethod<float()> getTextSize{this, "getTextSize"};
        JavaMethod<void(int)> setTextAppearance{this, "setTextAppearance"};

        JavaMethod<void(TextWatcher)> addTextChangedListener{this, "addTextChangedListener"};
        JavaMethod<void(TextWatcher)> removeTextChangedListener{this, "removeTextChangedListener"};

        JavaMethod<void(OnEditorActionListener)> setOnEditorActionListener{this, "setOnEditorActionListener"};
    };

    using TextView = BaseTextView<>;
}
