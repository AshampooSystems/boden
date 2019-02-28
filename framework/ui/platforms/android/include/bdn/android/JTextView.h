#pragma once

#include <bdn/android/JContext.h>
#include <bdn/android/JTextPaint.h>
#include <bdn/android/JTextWatcher.h>
#include <bdn/android/JView.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace android
    {

        static constexpr char kOnEditorActionListenerName[] = "android/widget/TextView$OnEditorActionListener";
        using OnEditorActionListener = bdn::java::JTObject<kOnEditorActionListenerName>;

        constexpr const char kTextViewClassName[] = "android/widget/TextView";

        template <const char *javaClassName = kTextViewClassName, class... ConstructorArguments>
        class JBaseTextView : public JBaseView<javaClassName, ConstructorArguments...>
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
            using JBaseView<javaClassName, ConstructorArguments...>::JBaseView;

            java::Method<JTextPaint()> getPaint{this, "getPaint"};

            java::Method<void(java::JCharSequence)> setText{this, "setText"};
            java::Method<java::JCharSequence()> getText{this, "getText"};

            java::Method<void(bool)> setSingleLine{this, "setSingleLine"};

            java::Method<void(int)> setBreakStrategy{this, "setBreakStrategy"};

            java::Method<void(bool)> setHorizontallyScrolling{this, "setHorizontallyScrolling"};

            java::Method<void(int)> setWidth{this, "setWidth"};
            java::Method<void(int)> setMaxWidth{this, "setMaxWidth"};
            java::Method<void(int)> setMaxHeight{this, "setMaxHeight"};
            java::Method<void(int)> setMaxLines{this, "setMaxLines"};

            java::Method<float()> getTextSize{this, "getTextSize"};
            java::Method<void(int)> setTextAppearance{this, "setTextAppearance"};

            java::Method<void(JTextWatcher)> addTextChangedListener{this, "addTextChangedListener"};
            java::Method<void(JTextWatcher)> removeTextChangedListener{this, "removeTextChangedListener"};

            java::Method<void(OnEditorActionListener)> setOnEditorActionListener{this, "setOnEditorActionListener"};
        };

        using JTextView = JBaseTextView<>;
    }
}
