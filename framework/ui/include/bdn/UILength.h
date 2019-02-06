#pragma once

#include <ostream>

namespace bdn
{

    /** Represents a length or distance to use sizing and spacing user interface
       elements.

        UILength objects can also be "none", which means that they represent a
       non-existent value (similar to the standard nullptr value).
       Default-constructed UILength objects are none.
        */
    struct UILength
    {
      public:
        enum class Unit
        {
            /** Special unit that indicates a "null" length. I.e. a UILength
               object that has no value.

                The value variable is ignored if the unit is none.
            */
            none,

            /** A "device independent pixel". This unit corresponds roughly to
               the perceived size of a pixel on an legacy 96 dpi desktop
               monitor.

                See \ref dip.md for more information.

                DIP is the default unit for Boden.
                */
            dip,

            /** 1 sem equals the height of the screen's default UI font.

                Use this when you want a length to depend on the default text
               size.
                */
            sem,

            /** 1 em is the height of the active font. Which font this is
               depends on where the UILength element is used. For example, if
               the UILength object is assigned to a View object then this refers
               to the font of the view.

                Use this when you want a length to scale with the text size of
               the UI element.
                */
            em

        };

        /** Default constructor - sets the unit to #UILength::Unit::none and
         * value to 0.*/
        UILength()
        {
            unit = UILength::Unit::none;
            value = 0;
        };

        UILength(double value, Unit unit = UILength::Unit::dip)
        {
            this->value = value;
            this->unit = unit;
        }

        /** Creates a UILength object with the specified value and the
         * UILength::Unit::sem unit.*/
        static UILength sem(double value) { return UILength(value, UILength::Unit::sem); }

        /** Creates a UILength object with the specified value and the
         * UILength::Unit::em unit.*/
        static UILength em(double value) { return UILength(value, UILength::Unit::em); }

        /** Creates a UILength object with the specified value and the
         * UILength::Unit::dip unit.*/
        static UILength dip(double value) { return UILength(value, UILength::Unit::dip); }

        /** Creates a UILength object with the UILength::Unit::none unit (same
         * as default-constructed UILength).*/
        static UILength none() { return UILength(); }

        /** Returns true if this UILength object has the special "none" value.*/
        bool isNone() const { return (unit == UILength::Unit::none); }

        bool operator==(const UILength &other) const { return unit == other.unit && value == other.value; }

        bool operator!=(const UILength &other) const { return !operator==(other); }

        Unit unit;
        double value;
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const UILength &l)
    {
        if (l.unit == UILength::Unit::none)
            return stream << "none";
        else {
            stream << l.value;

            if (l.unit == UILength::Unit::dip)
                stream << " dip";
            else if (l.unit == UILength::Unit::sem)
                stream << " sem";
            else if (l.unit == UILength::Unit::em)
                stream << " em";

            return stream;
        }
    }
}
