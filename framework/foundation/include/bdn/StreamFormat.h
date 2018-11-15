#ifndef BDN_StreamFormat_H_
#define BDN_StreamFormat_H_

namespace bdn
{

    /** Manipulates the formatting of a C++ standard library I/O stream (any
       stream derived directly or indirectly from std::ios_base ). This includes
       standard streams like std::cout and std::cin, std::fstream, etc.

        Note that the Boden \ref StringBuffer class is also a standard I/O
       stream, so this can be used with that as well.

        StreamFormat is designed to only influence the formatting of the very
       next value that is pushed into the output stream. The formatting is
       automatically restored to whatever it was before after the value was
       written.

        StreamFormat instances are often created on the fly as temporary objects
       and then immediately pushed into a stream with the << operator, followed
       by the value that is to be formatted.

        Usage example:

        \code

        // we can use any stream derived from std::basic_ostream. In this
       example
        // we use std::stringstream.
        std::stringstream stream;

        // use a temporary StreamFormat instance to format the very next value.
        // here the formatting will ONLY apply to the first value (15), which
        // will be printed in hexadecimal format and padded with zeros to 4
       places.
        // After the 15 has been written, the formatting is restored, so the
        // 42 is written in normal decimal format without zero padding.
        stream << StreamFormat().hex().zeroPad(4) << 15 << " " << 42;

        // The output is:
        // 000f 42

        // You can also store the format object to be able to use it multiple
       times: StreamFormat numberFormat = StreamFormat().hex().zeroPad(4);

        // this will apply the format to both the 15 and the 42.
        stream << numberFormat << 15 << " " << numberFormat << 42;

        \endcode
    */
    class StreamFormat
    {
      public:
        StreamFormat() {}

        /** Activates hexadecimal number output (for example "789a"). By
           default, the numbers are NOT written
            with a "0x" prefix. If you want that call hex0x() instead.*/
        StreamFormat &hex() & { return setBaseField(std::ios_base::hex); }

        StreamFormat &&hex() && { return std::move(setBaseField(std::ios_base::hex)); }

        /** Activates hexadecimal number output with a "0x" prefix (for example
           "0x789a"). If you do not want the "0x" prefix then call hex()
           instead.
        */
        StreamFormat &hex0x() &
        {
            return setFlags(std::ios_base::hex | std::ios_base::showbase,
                            std::ios_base::basefield | std::ios_base::showbase);
        }

        StreamFormat &&hex0x() &&
        {
            return std::move(setFlags(std::ios_base::hex | std::ios_base::showbase,
                                      std::ios_base::basefield | std::ios_base::showbase));
        }

        /** Pads the written value to the specified number of characters
            using zeros.

            For numbers the padding is inserted at the appropriate place
            (e.g. if the number is negative the padding is inserted after the
            minus sign).
            */
        StreamFormat &zeroPad(int minChars) &
        {
            _padTo = minChars;
            _padChar = U'0';
            return setAdjustField(std::ios_base::internal);
        }

        StreamFormat &&zeroPad(int minChars) &&
        {
            _padTo = minChars;
            _padChar = U'0';
            return std::move(setAdjustField(std::ios_base::internal));
        }

        /** Right-aligns the written value.

            \c columnWidthChars is the width in characters of the area
            in which the alignment occurs (i.e. the width of the column if you
            are outputting a table). The next written value will be padded on
           the left until it is at least columnWidthChars long.

            By default the padding is done with spaces. If you want to pad with
           a different character then specify it as the second parameter (\c
           padChar). Note that if you want to fill numbers up with zeros to a
           certain length then you should use zeroPad() instead, since that
           inserts the zeros at the correct place inside the number.
        */
        StreamFormat &alignRight(int columnWidthChars, char32_t padChar = U' ') &
        {
            _padTo = columnWidthChars;
            _padChar = padChar;

            return setAdjustField(std::ios_base::right);
        }

        StreamFormat &&alignRight(int columnWidthChars, char32_t padChar = U' ') &&
        {
            _padTo = columnWidthChars;
            _padChar = padChar;

            return std::move(setAdjustField(std::ios_base::right));
        }

        /** Left-aligns the written value. I.e. writes padding characters after
           the value until the desired length is reached.

            \c columnWidthChars is the width in characters of the area
            in which the alignment occurs (i.e. the width of the column if you
            are outputting a table). Padding will be written after the written
           value until it is at least columnWidthChars long.

            By default the padding is done with spaces. If you want to pad with
           a different character then specify it as the second parameter (\c
           padChar). Note that if you want to fill numbers up with zeros to a
           certain length then you should use zeroPad() instead, since that
           inserts the zeros at the correct place inside the number.
        */
        StreamFormat &alignLeft(int columnWidthChars, char32_t padChar = U' ') &
        {
            _padTo = columnWidthChars;
            _padChar = padChar;

            return setAdjustField(std::ios_base::left);
        }

        StreamFormat &&alignLeft(int columnWidthChars, char32_t padChar = U' ') &&
        {
            _padTo = columnWidthChars;
            _padChar = padChar;

            return std::move(setAdjustField(std::ios_base::left));
        }

        /** Causes floating point numbers to be written in either simple
           notation or scientific notation, based on the size of the number.

            The maxDigits parameter indicates the number of digits to use at
           most. If the part of the number before the decimal point can be
           expressed with the specified number of digits then the simple
           notation is used (e.g. 123.456). If the number is so big that it
           cannot be expressed with the specified number of digits then
           scientific notation is used (e.g. 1.23456e+03).

            If the number can be fully expressed with less than maxDigits digits
            then less digits will be used. Integer values are printed without a
           decimal point.

            The default format for floating point numbers is autoFloat(6).
        */
        StreamFormat &autoFloat(int maxDigits = 6) &
        {
            _precision = maxDigits;

            return setFloatField((std::ios_base::fmtflags)0);
        }

        /** Causes floating point numbers to always be written in scientific
           notation, using an exponent (e.g. "1.234e+07"), with the specified
           number of digits after the point.

            The specified number of places are always printed, even if the
           number can be expressed with less digits.

            The default format for floating point numbers is autoFloat(6).
        */
        StreamFormat &scientificFloat(int placesAfterPoint = 6) &
        {
            _precision = placesAfterPoint;

            return setFloatField(std::ios_base::scientific);
        }

        StreamFormat &&scientificFloat(int placesAfterPoint = 6) &&
        {
            _precision = placesAfterPoint;

            return std::move(setFloatField(std::ios_base::scientific));
        }

        /** Causes floating point numbers to always be written in simple
           notation (without using an exponent) and with the specified number of
           digits after the decimal point. Note that the specified number of
           digits after the point are always written, even if the number can be
           expressed with less.

            The default format for floating point numbers is autoFloat(6).
        */
        StreamFormat &simpleFloat(int digitsAfterPoint = 2) &
        {
            _precision = digitsAfterPoint;
            return setFloatField(std::ios_base::fixed);
        }

        StreamFormat &&simpleFloat(int digitsAfterPoint = 2) &&
        {
            _precision = digitsAfterPoint;
            return std::move(setFloatField(std::ios_base::fixed));
        }

        /** Sets the precision with which floating point numbers are written.

            It is recommended to use autoFloat(), scientificFloat() or
           simpleFloat() instead of setting the precision manually.

            The meaning of the precision setting depends on the floating point
           format that is used. If automatic format is used (the default - see
           autoFloat()) then the precision refers to the total number of digits
           to use to print the number (the sum of the decimal places before and
           after the point). If the number can be fully expressed with less
           decimal places then less are used.

            If scientific or simple format are used then the precision refers to
           the number of decimal places AFTER the point only. Also, the
           specified number of decimal places are ALWAYS written, even if they
           are not necessary.

            */
        StreamFormat &setPrecision(int prec) &
        {
            _precision = prec;
            return *this;
        }

        StreamFormat &&setPrecision(int prec) &&
        {
            _precision = prec;
            return std::move(*this);
        }

        /** Returns the precision that has been set for this format.
            Returns -1 if the precision has not been set explicitly and the
           default precision is used.*/
        int precision() const { return _precision; }

        /** Forces floating point numbers to be written with a point, even if
           they do not have a fractional component. If forcePoint() is called
           then 123 is written as "123.0".*/
        StreamFormat &forcePoint() & { return setFlag(std::ios_base::showpoint); }

        StreamFormat &&forcePoint() && { return std::move(setFlag(std::ios_base::showpoint)); }

        /** Sets the length that values are padded to (in characters).
            zeroPad(), alignLeft() or alignRight() are usually better
           alternatives for common use cases.
        */
        StreamFormat &setPadTo(int chars) &
        {
            _padTo = chars;
            return *this;
        }

        StreamFormat &&setPadTo(int chars) &&
        {
            _padTo = chars;
            return std::move(*this);
        }

        /** Returns the number of characters that values are padded to.

            Returns -1 if no padding length was set and the stream's padding
           length will not be modified. That usually means that padding will not
           take place, unless the stream's padding length was set using other
           means.
            */
        int padTo() const { return _padTo; }

        /** Sets the character that is used to pad written values to the desired
            length.
            zeroPad(), alignLeft() or alignRight() are usually better
           alternatives for common use cases.
        */
        StreamFormat &setPadChar(char32_t padChar) &
        {
            _padChar = padChar;
            return *this;
        }

        StreamFormat &&setPadChar(char32_t padChar) &&
        {
            _padChar = padChar;
            return std::move(*this);
        }

        enum : char32_t
        {
            padCharNotSet = 0xffffffff
        };

        /** Returns the character that is used to pad written values (if padding
           is enabled).

            Returns padCharNotSet if the padding character was not set
           explicitly and the stream's padding character will not be modified.
            If the stream's padding character was not changed using other means
           then it is ' ' by default in this case.
            */
        char32_t padChar() const { return _padChar; }

        /** Sets a low level formatting flag (see std::basic_ios::setf).
            It is usually simpler to use one of the high level functions (like
           hex0x()) instead of manually setting flags.

            \c flags contains the flag values

            \c mask contains a bitmask of the flag area that should be
           overwritten with those from \c flags. When the format is applied to a
           stream object then only those of the stream's flags that are covered
            by \c mask will be modified and set to the corresponding bit value
           from \c flags.
            */
        StreamFormat &setFlags(std::ios_base::fmtflags flags, std::ios_base::fmtflags mask) &
        {
            _flags &= ~mask;
            _flags |= flags;
            _flagsMask |= mask;
            return *this;
        }

        StreamFormat &&setFlags(std::ios_base::fmtflags flags, std::ios_base::fmtflags mask) &&
        {
            _flags &= ~mask;
            _flags |= flags;
            _flagsMask |= mask;
            return std::move(*this);
        }

        /** Sets an individual flag. Same as setFlags(flag, flag).*/
        StreamFormat &setFlag(std::ios_base::fmtflags flag) &
        {
            _flags |= flag;
            _flagsMask |= flag;
            return *this;
        }

        StreamFormat &&setFlag(std::ios_base::fmtflags flag) &&
        {
            _flags |= flag;
            _flagsMask |= flag;
            return std::move(*this);
        }

        /** Returns the flags that have been set explicitly.

            Note that flagsMask() returns a bit mask of the flag areas that have
           been explicitly modified. When the format is applied to a stream then
           only those flags of the stream that are covered by the mask will be
           overwritten.*/
        std::ios_base::fmtflags flags() const { return _flags; }

        /** Returns the bitmask that covers all flags that have been explicitly
            initialized in the format.

            When the format is applied to a stream then only those flags of the
           stream that are covered by the mask will be overwritten.*/
        std::ios_base::fmtflags flagsMask() const { return _flagsMask; }

      private:
        StreamFormat &setAdjustField(std::ios_base::fmtflags flag) &
        {
            return setFlags(flag, std::ios_base::adjustfield);
        }

        StreamFormat &&setAdjustField(std::ios_base::fmtflags flag) &&
        {
            return std::move(setFlags(flag, std::ios_base::adjustfield));
        }

        StreamFormat &setBaseField(std::ios_base::fmtflags flag) & { return setFlags(flag, std::ios_base::basefield); }

        StreamFormat &&setBaseField(std::ios_base::fmtflags flag) &&
        {
            return std::move(setFlags(flag, std::ios_base::basefield));
        }

        StreamFormat &setFloatField(std::ios_base::fmtflags flag) &
        {
            return setFlags(flag, std::ios_base::floatfield);
        }

        StreamFormat &&setFloatField(std::ios_base::fmtflags flag) &&
        {
            return std::move(setFlags(flag, std::ios_base::floatfield));
        }

        int _padTo = -1;
        char32_t _padChar = padCharNotSet;
        int _precision = -1;
        std::ios_base::fmtflags _flags = (std::ios_base::fmtflags)0;
        std::ios_base::fmtflags _flagsMask = (std::ios_base::fmtflags)0;
    };

    template <class STREAM_TYPE> class StreamFormatApplier_
    {
      public:
        StreamFormatApplier_(STREAM_TYPE &stream, const StreamFormat &format) : _stream(stream), _format(format) {}

        template <typename VALUE_TYPE> STREAM_TYPE &operator<<(VALUE_TYPE &&value)
        {
            TemporaryFormatChanger formatChanger(_stream, _format);

            _stream << std::forward<VALUE_TYPE>(value);

            return _stream;
        }

      private:
        class TemporaryFormatChanger
        {
          public:
            TemporaryFormatChanger(STREAM_TYPE &stream, const StreamFormat &format) : _stream(stream)
            {
                if (format.padTo() != -1)
                    _oldPadTo = (int)_stream.width(format.padTo());

                if (format.padChar() != StreamFormat::padCharNotSet)
                    _oldPadChar = _stream.fill((typename STREAM_TYPE::char_type)format.padChar());

                if (format.precision() != -1)
                    _oldPrecision = (int)_stream.precision(format.precision());

                _flagsMask = format.flagsMask();
                if (_flagsMask != 0)
                    _oldFlags = _stream.setf(format.flags(), _flagsMask);
            }

            ~TemporaryFormatChanger()
            {
                if (_oldPadTo != -1)
                    _stream.width(_oldPadTo);

                if (_oldPadChar != -1)
                    _stream.fill(_oldPadChar);

                if (_oldPrecision != -1)
                    _stream.precision(_oldPrecision);

                if (_flagsMask != -0)
                    _stream.setf(_oldFlags, _flagsMask);
            }

            STREAM_TYPE &_stream;
            int _oldPadTo = -1;
            int _oldPadChar = -1;
            int _oldPrecision = -1;
            std::ios_base::fmtflags _oldFlags = (std::ios_base::fmtflags)0;
            std::ios_base::fmtflags _flagsMask = (std::ios_base::fmtflags)0;
        };

        STREAM_TYPE &_stream;
        StreamFormat _format;
    };

    template <class STREAM_CHAR_TYPE, class STREAM_TRAITS>
    StreamFormatApplier_<std::basic_ostream<STREAM_CHAR_TYPE, STREAM_TRAITS>>
    operator<<(std::basic_ostream<STREAM_CHAR_TYPE, STREAM_TRAITS> &stream, const StreamFormat &format)
    {
        return StreamFormatApplier_<std::basic_ostream<STREAM_CHAR_TYPE, STREAM_TRAITS>>(stream, format);
    }
}

#endif
