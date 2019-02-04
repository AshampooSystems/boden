#pragma once

#include <bdn/android/JView.h>
#include <bdn/android/JContext.h>
#include <bdn/android/JTextPaint.h>
#include <bdn/android/JTextWatcher.h>
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

            Method<JTextPaint()> getPaint{this, "getPaint"};

            Method<void(JCharSequence)> setText{this, "setText"};
            Method<JCharSequence()> getText{this, "getText"};

            Method<void(bool)> setSingleLine{this, "setSingleLine"};

            Method<void(int)> setBreakStrategy{this, "setBreakStrategy"};

            Method<void(bool)> setHorizontallyScrolling{this, "setHorizontallyScrolling"};

            Method<void(int)> setMaxLines{this, "setMaxLines"};

            Method<void(int)> setWidth{this, "setWidth"};
            Method<void(int)> setMaxWidth{this, "setMaxWidth"};
            Method<void(int)> setMaxHeight{this, "setMaxHeight"};

            Method<float()> getTextSize{this, "getTextSize"};
            Method<void(int)> setTextAppearance{this, "setTextAppearance"};

            Method<void(JTextWatcher)> addTextChangedListener{this, "addTextChangedListener"};
            Method<void(JTextWatcher)> removeTextChangedListener{this, "removeTextChangedListener"};

            Method<void(OnEditorActionListener)> setOnEditorActionListener{this, "setOnEditorActionListener"};
        };

        using JTextView = JBaseTextView<>;
    }
}
