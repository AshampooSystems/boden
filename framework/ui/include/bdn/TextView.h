#ifndef BDN_TextView_H_
#define BDN_TextView_H_

#include <bdn/View.h>
#include <bdn/ITextViewCore.h>

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
        TextView() {}

        /** Returns the TextView's text content.
         */
        BDN_VIEW_PROPERTY(String, text, setText, ITextViewCore, influencesPreferredSize());

        /* * Can be used to give the text view a hint as to what the preferred
        width or height should be.

            If width or height are -1 then it means "unspecified" and the text
        view will automatically calculate an optimal value for the missing
        component.

            It is recommended that you set a hint for the preferred width of a
        text view to give the view a hint at what width it should start to
        automatically wrap its text into multiple lines. It is usually best to
        set the height component of the preferred size hint to -1.

        Property<UiSize>& preferredSizeHint()
        {
            return _preferredSizeHint;
        }

        const ReadProperty<UiSize>& preferredSizeHint() const
        {
            return _preferredSizeHint;
        }*/

        /** Static function that returns the type name for text view core
         * objects.*/
        static String getTextViewCoreTypeName() { return "bdn.TextViewCore"; }

        String getCoreTypeName() const override { return getTextViewCoreTypeName(); }

      protected:
    };
}

#endif
