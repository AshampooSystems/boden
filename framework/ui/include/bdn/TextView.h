#pragma once

#include <bdn/ITextViewCore.h>
#include <bdn/View.h>

namespace bdn
{

    /** Displays text, but does not offer the option to edit it.
        See the TextField class if you need text entering capability.

        The text view will automatically wrap text into multiple lines if it
       does not fit into the view otherwise. The wrapping occurs at word
       boundaries. If a single word does not fit in the view then the behaviour
       can differ, depending on the platform / implementation. Most
       implementations will clip the word (the recommended behaviour), but some
       implementations may wrap at character boundaries inside the word, thus
       splitting the word to multiple lines.

        With default settings, the "preferred size" of the text view will be
       chosen so that no text has to be wrapped. For long texts this is usually
       not optimal. For that reason it is recommended that you set a
       preferredSizeHint() to let the text view know what its preferred width
       should be (roughly). When such a hint is set then the text view will
       calculate a preferred size that comes as close to that hint as possible.
        */
    class TextView : public View
    {
      public:
        Property<String> text;
        Property<bool> wrap;

      public:
        TextView();

      public:
        static constexpr char coreTypeName[] = "bdn.TextViewCore";
        String getCoreTypeName() const override { return coreTypeName; }
    };
}
