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
        using BaseView<javaClassName, ConstructorArguments...>::BaseView;

        JavaMethod<TextPaint()> getPaint{this, "getPaint"};

        JavaMethod<void(java::wrapper::CharSequence)> setText{this, "setText"};
        JavaMethod<java::wrapper::CharSequence()> getText{this, "getText"};

        JavaMethod<void(bool)> setSingleLine{this, "setSingleLine"};

        JavaMethod<void(int)> setBreakStrategy{this, "setBreakStrategy"}; // Layout::BREAK_STRATEGY_...

        JavaMethod<void(bool)> setHorizontallyScrolling{this, "setHorizontallyScrolling"};

        JavaMethod<void(int)> setWidth{this, "setWidth"};
        JavaMethod<void(int)> setMaxWidth{this, "setMaxWidth"};
        JavaMethod<void(int)> setMaxHeight{this, "setMaxHeight"};
        JavaMethod<void(int)> setMaxLines{this, "setMaxLines"};

        JavaMethod<float()> getTextSize{this, "getTextSize"};
        JavaMethod<void(int)> setTextAppearance{this, "setTextAppearance"};

        JavaMethod<void(int)> setInputType{this, "setInputType"};
        JavaMethod<int()> getInputType{this, "getInputType"};

        JavaMethod<void(TextWatcher)> addTextChangedListener{this, "addTextChangedListener"};
        JavaMethod<void(TextWatcher)> removeTextChangedListener{this, "removeTextChangedListener"};

        JavaMethod<void(OnEditorActionListener)> setOnEditorActionListener{this, "setOnEditorActionListener"};
    };

    using TextView = BaseTextView<>;
}
