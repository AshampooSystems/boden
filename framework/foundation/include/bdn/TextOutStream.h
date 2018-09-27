#ifndef BDN_TextOutStream_H_
#define BDN_TextOutStream_H_

#include <bdn/localeUtil.h>
#include <bdn/LocaleDecoder.h>
#include <bdn/typeUtil.h>

#include <ostream>

namespace bdn
{

    /** Character traits type for 32 bit unicode character streams.

        This type currently behaves identical to std::char_traits<char32_t>, but
       it is a different type.

        The existence of this type has fairly technical reasons that most users
       of the Boden framework do not need to be concerned about. See the
       documentation of bdn::TextOutStream.

        */
    class UnicodeCharTraits : public std::char_traits<char32_t>
    {
      public:
    };

    /** Writes a character sequence to an output stream, just like a normal
       string would be written. Alignment and padding are properly handled
       according to the stream formatting options, in the same way as they would
       be if a normal string object was written.

        The character sequence is specified as an iterator interval: the beginIt
       iterator points to the first character of the sequence and endIt points
       to the position just after the last character in the sequence. The
       iterators must return characters of the same type that the stream uses
       (CHAR_TYPE).
    */
    template <typename CHAR_TYPE, class CHAR_TRAITS, typename IT_TYPE>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &
    streamPutCharSequence(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                          const IT_TYPE &beginIt, const IT_TYPE &endIt)
    {
        typename std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>::sentry s(stream);
        if (s) {
            try {
                auto pBuffer = stream.rdbuf();
                if (pBuffer == nullptr)
                    stream.setstate(std::ios_base::badbit);
                else {
                    std::streamsize padTo = stream.width();
                    bool leftAlign =
                        ((stream.flags() & std::ios_base::adjustfield) ==
                         std::ios_base::left);

                    bool ok = true;
                    if (padTo > 0 && !leftAlign) {
                        // if we right-align then we must first know how many
                        // characters we are about to write.
                        size_t length = std::distance(beginIt, endIt);
                        for (size_t i = length; i < (size_t)padTo; i++) {
                            if (CHAR_TRAITS::eq_int_type(
                                    pBuffer->sputc(stream.fill()),
                                    CHAR_TRAITS::eof())) {
                                stream.setstate(std::ios_base::badbit);
                                ok = false;
                                break;
                            }
                        }
                    }

                    if (ok) {
                        size_t written = 0;
                        for (IT_TYPE it = beginIt; it != endIt; ++it) {
                            if (CHAR_TRAITS::eq_int_type(pBuffer->sputc(*it),
                                                         CHAR_TRAITS::eof())) {
                                stream.setstate(std::ios_base::badbit);
                                ok = false;
                                break;
                            }

                            written++;
                        }

                        if (ok && leftAlign && padTo > 0) {
                            while (written < (size_t)padTo) {
                                if (CHAR_TRAITS::eq_int_type(
                                        pBuffer->sputc(stream.fill()),
                                        CHAR_TRAITS::eof())) {
                                    stream.setstate(std::ios_base::badbit);
                                    ok = false;
                                    break;
                                }

                                written++;
                            }
                        }
                    }
                }

                // must reset width to 0 (that is how streams do it).
                stream.width(0);
            }
            catch (...) {
                stream.setstate(std::ios_base::badbit);
            }

            if (stream.rdstate() & stream.exceptions())
                throw typename std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>::
                    failure("Error writing character sequence.");
        }

        return stream;
    }
}

namespace std
{

    // we have to provide a specialization for basic_ios::widen and narrow
    // because the default implementation uses std::ctype<char32_t> in some
    // implementations - and that does not always exist
    template <>
    inline char32_t
    basic_ios<char32_t, bdn::UnicodeCharTraits>::widen(char c) const
    {
        return (char32_t)use_facet<ctype<wchar_t>>(getloc()).widen(c);
    }

    template <>
    inline char
    basic_ios<char32_t, bdn::UnicodeCharTraits>::narrow(char32_t c,
                                                        char defChar) const
    {
        return use_facet<ctype<wchar_t>>(getloc()).narrow((wchar_t)c, defChar);
    }

    /** An implementation for 32 bit character output streams.
        (specialization of basic_ostream< char32_t, bdn::UnicodeCharTraits> )

        See bdn::TextOutStream
        */
    template <>
    class basic_ostream<char32_t, bdn::UnicodeCharTraits>
        : public std::basic_ios<char32_t, bdn::UnicodeCharTraits>
    {
        using BasicIos =
            typename std::basic_ios<char32_t, bdn::UnicodeCharTraits>;

      public:
        using typename BasicIos::char_type;
        using typename BasicIos::int_type;
        using typename BasicIos::off_type;
        using typename BasicIos::pos_type;
        using typename BasicIos::traits_type;

        /** Returns the locale that the stream uses by default.
            This is a variant of the classic locale (std::locale::classic() )
            for which the "char" encoding was replaced with UTF-8.*/
        static std::locale defaultLocale()
        {
            static std::locale _loc =
                bdn::deriveUtf8Locale(std::locale::classic());
            return _loc;
        }

        explicit basic_ostream(
            std::basic_streambuf<char_type, traits_type> *pBuffer)
            : BasicIos(pBuffer)
        {
            // by default we use a variant of the classic locale, with the
            // multibyte encoding replaced with UTF-8.
            imbue(defaultLocale());

            // we know that the default locale is Utf-8
            _lastCheckedLocale = BasicIos::getloc();
            _lastCheckedLocaleIsUtf8 = 1;

            _pWideAdapterStreamBuffer = new WideAdapterStreamBuffer(this);

            BasicIos::setf(std::ios_base::boolalpha);
        }

      protected:
        basic_ostream()
        {
            _lastCheckedLocaleIsUtf8 = -1;

            _pWideAdapterStreamBuffer = new WideAdapterStreamBuffer(this);
        }

        void
        init(std::basic_streambuf<char32_t, bdn::UnicodeCharTraits> *pBuffer)
        {
            BasicIos::init(pBuffer);

            // by default we use a variant of the classic locale, with the
            // multibyte encoding replaced with UTF-8.
            imbue(defaultLocale());

            // we know that the default locale is Utf-8
            _lastCheckedLocale = BasicIos::getloc();
            _lastCheckedLocaleIsUtf8 = 1;

            BasicIos::setf(std::ios_base::boolalpha);
        }

        basic_ostream(const basic_ostream &rhs) = delete;

        basic_ostream(basic_ostream &&rhs)
            : _lastCheckedLocale(std::move(rhs._lastCheckedLocale)),
              _lastCheckedLocaleIsUtf8(rhs._lastCheckedLocaleIsUtf8)
        {
            rhs._lastCheckedLocaleIsUtf8 = -1;

            _pWideAdapterStreamBuffer = rhs._pWideAdapterStreamBuffer;
            rhs._pWideAdapterStreamBuffer = nullptr;
            _pWideAdapterStreamBuffer->setStream(this);

            // we apparently must call init before move. Otherwise
            // we will get a crash in the destructor.
            BasicIos::init(nullptr);
            BasicIos::move(std::move(rhs));
        }

      public:
        virtual ~basic_ostream()
        {
            if (_pWideAdapterStreamBuffer != nullptr)
                delete _pWideAdapterStreamBuffer;
        }

      protected:
        basic_ostream &operator=(const basic_ostream &rhs) = delete;
        basic_ostream &operator=(basic_ostream &&rhs)
        {
            swap(rhs);
            return *this;
        }

      private:
        bool _hexOrOct()
        {
            return (BasicIos::flags() & ios_base::basefield) == ios_base::oct ||
                   (BasicIos::flags() & ios_base::basefield) == ios_base::hex;
        }

      public:
        class sentry
        {
          public:
            explicit sentry(std::basic_ostream<char_type, traits_type> &stream)
                : _stream(stream)
            {
                _preparationGood = stream.good();
                if (_preparationGood) {
                    auto t = stream.tie();
                    if (t != nullptr) {
                        t->flush();
                        _preparationGood = stream.good();
                    }
                }
            }

            ~sentry()
            {
                if ((_stream.flags() & std::ios_base::unitbuf) &&
                    !std::uncaught_exception() && _stream.good()) {
                    auto pBuffer = _stream.rdbuf();
                    if (pBuffer != nullptr && pBuffer->pubsync() == -1)
                        _stream.setstate(std::ios_base::badbit);
                }
            }

            explicit operator bool() const { return _preparationGood; }

            sentry &operator=(const sentry &) = delete;

          private:
            std::basic_ostream<char_type, traits_type> &_stream;
            bool _preparationGood;
        };

        basic_ostream &operator<<(short value)
        {
            if (_hexOrOct())
                return operator<<((unsigned long)(unsigned short)value);
            else
                return operator<<((long)value);
        }

        basic_ostream &operator<<(unsigned short value)
        {
            return operator<<((unsigned long)value);
        }

        basic_ostream &operator<<(int value)
        {
            if (_hexOrOct())
                return operator<<((unsigned long)(unsigned int)value);
            else
                return operator<<((long)value);
        }

        basic_ostream &operator<<(unsigned int value)
        {
            return operator<<((unsigned long)value);
        }

      private:
        template <typename VALUE>
        basic_ostream &_doSentriedNumPut(VALUE value, ios_base &fmt,
                                         char32_t fillChar)
        {
            sentry s(*this);
            if (s) {
                std::locale loc = getloc();
                auto it = std::use_facet<std::num_put<wchar_t>>(loc).put(
                    ostreambuf_iterator<wchar_t>(_pWideAdapterStreamBuffer),
                    fmt, (wchar_t)fillChar, value);

                if (!_pWideAdapterStreamBuffer->syncToRealBuffer() ||
                    it.failed())
                    setstate(std::ios_base::badbit);
            }

            return *this;
        }

      public:
        basic_ostream &operator<<(long value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(unsigned long value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(long long value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(unsigned long long value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(float value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(double value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(long double value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(bool value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(const void *value)
        {
            return _doSentriedNumPut(value, *this, fill());
        }

        basic_ostream &operator<<(std::nullptr_t) { return (*this) << U"null"; }

        basic_ostream &
        operator<<(std::basic_streambuf<char_type, traits_type> *pBuffer)
        {
            sentry s(*this);
            if (s) {
                if (pBuffer == nullptr)
                    setstate(std::ios_base::badbit);
                else {
                    try {
                        // if put fails then we MUST NOT extract the input
                        // character. So we read first without advancing and
                        // then advance afterwards.

                        int_type chr = pBuffer->sgetc();
                        if (traits_type::eq_int_type(chr, traits_type::eof())) {
                            // if no characters are inserted (if the input
                            // buffer is empty) then we must set failbit
                            setstate(ios_base::failbit);
                        } else {
                            do {
                                put(chr);
                                if (!good())
                                    break;

                                chr = pBuffer->snextc();
                            } while (!traits_type::eq_int_type(
                                chr, traits_type::eof()));
                        }
                    }
                    catch (...) {
                        setstate(ios_base::failbit);

                        if (exceptions() & ios_base::failbit)
                            throw;
                    }
                }
            }

            return *this;
        }

        basic_ostream &operator<<(std::ios_base &(*func)(std::ios_base &))
        {
            func(*this);
            return *this;
        }

        basic_ostream &operator<<(std::basic_ios<char_type, traits_type> &(
            *func)(std::basic_ios<char_type, traits_type> &))
        {
            func(*this);
            return *this;
        }

        basic_ostream &operator<<(std::basic_ostream<char_type, traits_type> &(
            *func)(std::basic_ostream<char_type, traits_type> &))
        {
            func(*this);
            return *this;
        }

        // the following are nonstandard operator overloads that the normal
        // std::basic_ostream does not provide. We provide this because in
        // contrast with the normal basic_ostream we want to properly decode
        // char strings according to the locale encoding. So we do not want the
        // global generic template implementation of this to be used.

        basic_ostream &operator<<(const char *s)
        {
            if (usingUtf8Encoding()) {
                // use our own UTF-8 decoding routines rather than relying on
                // those from the locale (the standard library decoding can be
                // quite buggy with some standard libraries - especially when
                // incorrectly encoded data is encountered). So, to get
                // consistent behaviour across all platforms and compilers we
                // decode ourselves
                const char *endOfS = s + std::char_traits<char>::length(s);

                bdn::Utf8Codec::DecodingIterator<const char *> beginIt(s, s,
                                                                       endOfS);
                bdn::Utf8Codec::DecodingIterator<const char *> endIt(endOfS, s,
                                                                     endOfS);

                return bdn::streamPutCharSequence(*this, beginIt, endIt);
            } else {
                bdn::LocaleDecoder decoder(s, std::char_traits<char>::length(s),
                                           BasicIos::getloc());

                return bdn::streamPutCharSequence(*this, decoder.begin(),
                                                  decoder.end());
            }
        }

        template <class STRING_TRAITS, class ALLOCATOR>
        basic_ostream &
        operator<<(const std::basic_string<char, STRING_TRAITS, ALLOCATOR> &s)
        {
            if (usingUtf8Encoding()) {
                bdn::Utf8Codec::DecodingIterator<typename std::basic_string<
                    char, STRING_TRAITS, ALLOCATOR>::const_iterator>
                    beginIt(s.begin(), s.begin(), s.end());

                bdn::Utf8Codec::DecodingIterator<typename std::basic_string<
                    char, STRING_TRAITS, ALLOCATOR>::const_iterator>
                    endIt(s.end(), s.begin(), s.end());

                return bdn::streamPutCharSequence(*this, beginIt, endIt);
            } else {
                bdn::LocaleDecoder decoder(s.c_str(), s.length(),
                                           BasicIos::getloc());

                return bdn::streamPutCharSequence(*this, decoder.begin(),
                                                  decoder.end());
            }
        }

        basic_ostream &operator<<(const wchar_t *s)
        {
            const wchar_t *endOfS = s + std::char_traits<wchar_t>::length(s);

            bdn::WideCodec::DecodingIterator<const wchar_t *> beginIt(s, s,
                                                                      endOfS);
            bdn::WideCodec::DecodingIterator<const wchar_t *> endIt(endOfS, s,
                                                                    endOfS);

            return bdn::streamPutCharSequence(*this, beginIt, endIt);
        }

        template <class STRING_TRAITS, class ALLOCATOR>
        basic_ostream &operator<<(
            const std::basic_string<wchar_t, STRING_TRAITS, ALLOCATOR> &s)
        {
            bdn::WideCodec::DecodingIterator<typename std::basic_string<
                wchar_t, STRING_TRAITS, ALLOCATOR>::const_iterator>
                beginIt(s.begin(), s.begin(), s.end());

            bdn::WideCodec::DecodingIterator<typename std::basic_string<
                wchar_t, STRING_TRAITS, ALLOCATOR>::const_iterator>
                endIt(s.end(), s.begin(), s.end());

            return bdn::streamPutCharSequence(*this, beginIt, endIt);
        }

        basic_ostream &operator<<(const char16_t *s)
        {
            const char16_t *endOfS = s + std::char_traits<char16_t>::length(s);

            bdn::Utf16Codec::DecodingIterator<const char16_t *> beginIt(s, s,
                                                                        endOfS);
            bdn::Utf16Codec::DecodingIterator<const char16_t *> endIt(endOfS, s,
                                                                      endOfS);

            return bdn::streamPutCharSequence(*this, beginIt, endIt);
        }

        template <class STRING_TRAITS, class ALLOCATOR>
        basic_ostream &operator<<(
            const std::basic_string<char16_t, STRING_TRAITS, ALLOCATOR> &s)
        {
            bdn::Utf16Codec::DecodingIterator<typename std::basic_string<
                char16_t, STRING_TRAITS, ALLOCATOR>::const_iterator>
                beginIt(s.begin(), s.begin(), s.end());

            bdn::Utf16Codec::DecodingIterator<typename std::basic_string<
                char16_t, STRING_TRAITS, ALLOCATOR>::const_iterator>
                endIt(s.end(), s.begin(), s.end());

            return bdn::streamPutCharSequence(*this, beginIt, endIt);
        }

        basic_ostream &operator<<(const char32_t *s)
        {
            const char32_t *endOfS = s + std::char_traits<char32_t>::length(s);

            return bdn::streamPutCharSequence(*this, s, endOfS);
        }

        template <class STRING_TRAITS, class ALLOCATOR>
        basic_ostream &operator<<(
            const std::basic_string<char32_t, STRING_TRAITS, ALLOCATOR> &s)
        {
            return bdn::streamPutCharSequence(*this, s.begin(), s.end());
        }

        basic_ostream &operator<<(char chr)
        {
            return bdn::streamPutCharSequence(*this, &chr, (&chr) + 1);
        }

        basic_ostream &operator<<(wchar_t chr)
        {
            return bdn::streamPutCharSequence(*this, &chr, (&chr) + 1);
        }

        basic_ostream &operator<<(char16_t chr)
        {
            return bdn::streamPutCharSequence(*this, &chr, (&chr) + 1);
        }

        basic_ostream &operator<<(char32_t chr)
        {
            return bdn::streamPutCharSequence(*this, &chr, (&chr) + 1);
        }

        basic_ostream &put(char_type ch)
        {
            sentry s(*this);
            if (s) {
                auto pBuffer = rdbuf();
                if (pBuffer == nullptr ||
                    traits_type::eq_int_type(pBuffer->sputc(ch),
                                             traits_type::eof())) {
                    setstate(ios_base::badbit);
                }
            }

            return *this;
        }

        basic_ostream &write(const char_type *s, std::streamsize count)
        {
            sentry sentr(*this);
            if (sentr) {
                try {
                    auto pBuffer = rdbuf();
                    if (pBuffer == nullptr ||
                        pBuffer->sputn(s, count) != count) {
                        setstate(ios_base::badbit);
                        if (exceptions() & ios_base::badbit)
                            throw failure("Error writing to stream");
                    }
                }
                catch (...) {
                    setstate(ios_base::badbit);
                    if (exceptions() & ios_base::badbit)
                        throw;
                }
            }

            return *this;
        }

        pos_type tellp()
        {
            auto pBuffer = rdbuf();
            if (pBuffer == nullptr)
                return pos_type(-1);

            if (fail())
                return pos_type(-1);
            else
                return pBuffer->pubseekoff(0, std::ios_base::cur,
                                           std::ios_base::out);
        }

        basic_ostream &seekp(pos_type pos)
        {
            auto pBuffer = rdbuf();
            if (pBuffer == nullptr ||
                pBuffer->pubseekpos(pos, std::ios_base::out) == pos_type(-1)) {
                setstate(ios_base::failbit);
                if (exceptions() & ios_base::failbit)
                    throw failure("seek failed");
            }

            return *this;
        }

        basic_ostream &seekp(off_type off, std::ios_base::seekdir dir)
        {
            auto pBuffer = rdbuf();
            if (pBuffer == nullptr ||
                pBuffer->pubseekoff(off, dir, std::ios_base::out) ==
                    pos_type(-1)) {
                setstate(ios_base::failbit);
                if (exceptions() & ios_base::failbit)
                    throw failure("seek failed");
            }

            return *this;
        }

        basic_ostream &flush()
        {
            auto pBuffer = rdbuf();
            if (pBuffer != nullptr) {
                if (pBuffer->pubsync() == pos_type(-1)) {
                    setstate(ios_base::badbit);
                    if (exceptions() & ios_base::badbit)
                        throw failure("flush failed");
                }
            }

            return *this;
        }

      protected:
        void swap(basic_ostream &rhs)
        {
            basic_ios::swap(rhs);

            std::swap(_pWideAdapterStreamBuffer, rhs._pWideAdapterStreamBuffer);
            _pWideAdapterStreamBuffer->setStream(this);
            rhs._pWideAdapterStreamBuffer->setStream(&rhs);
        }

      private:
        class WideAdapterStreamBuffer : public std::basic_streambuf<wchar_t>
        {
          private:
            using WideTraits = std::basic_streambuf<wchar_t>::traits_type;
            using UniTraits = basic_ostream::traits_type;

          public:
            WideAdapterStreamBuffer(basic_ostream *pStream) : _pStream(pStream)
            {
                setp(_wideChars, &_wideChars[capacity]);
            }

            void setStream(basic_ostream *pStream) { _pStream = pStream; }

            bool syncToRealBuffer()
            {
                // convert to full chars
                size_t count = pptr() - pbase();
                if (count == 0)
                    return true;
                else {
                    for (size_t i = 0; i < count; i++)
                        _uniChars[i] = _wideChars[i];

                    setp(_wideChars, &_wideChars[capacity]);

                    auto pBuffer = _pStream->rdbuf();
                    if (pBuffer == nullptr)
                        return false;
                    else
                        return (pBuffer->sputn(_uniChars, count) == count);
                }
            }

            void swap(WideAdapterStreamBuffer &other)
            {
                std::basic_streambuf<wchar_t>::swap(other);
            }

          protected:
            std::basic_streambuf<wchar_t>::int_type
            overflow(std::basic_streambuf<wchar_t>::int_type chr) override
            {
                if (!syncToRealBuffer())
                    return std::basic_streambuf<wchar_t>::traits_type::eof();

                if (WideTraits::eq_int_type(chr, WideTraits::eof())) {
                    // end of stream. notify real buffer
                    auto pBuffer = _pStream->rdbuf();
                    if (pBuffer != nullptr) {
                        // we can ignore the return value here, since we will
                        // ALWAYS return eof
                        pBuffer->sputc(UniTraits::eof());
                    }

                    return chr;
                } else
                    return sputc(chr);
            }

            int sync() override
            {
                if (!syncToRealBuffer())
                    return -1;

                auto pBuffer = _pStream->rdbuf();
                if (pBuffer == nullptr)
                    return 0;
                else
                    return pBuffer->pubsync();
            }

          private:
            enum
            {
                capacity = 16
            };

            basic_ostream *_pStream;
            char32_t _uniChars[capacity];
            wchar_t _wideChars[capacity];
        };

        bool usingUtf8Encoding()
        {
            if (_lastCheckedLocaleIsUtf8 == -1 ||
                _lastCheckedLocale != getloc()) {
                _lastCheckedLocale = getloc();

                _lastCheckedLocaleIsUtf8 =
                    bdn::isUtf8Locale(_lastCheckedLocale) ? 1 : 0;
            }

            return _lastCheckedLocaleIsUtf8 != 0;
        }

        WideAdapterStreamBuffer *_pWideAdapterStreamBuffer = nullptr;

        std::locale _lastCheckedLocale;
        int _lastCheckedLocaleIsUtf8 = -1;
    };
}

namespace bdn
{

    /** An output stream for unicode text (32 bit unicode characters -
       char32_t).

        This is an alias for std::basic_ostream<char32_t, UnicodeCharTraits>,
        which is a specialized version of basic_ostream.

        TextOutStream is used in the same way as other standard library I/O
       streams (like std::cout, std::stringstream, etc.), since it is a
       customized version of the standard library class std::basic_ostream. To
       write values and objects you simply push them into the stream with the <<
       operator. See examples below.

        The helper class StreamFormat can be used to control the formatting of
       the next written value.

        Locale independence
        -------------------

        By default, the formatting of TextOutStream does NOT depend on the
       global locale. Unless it is explicitly changed, the stream uses a locale
       with english formatting rules and UTF-8 encoding for char strings. The
       default locale is actually a variant of the "classic" locale (see
       std::locale::classic() ), in which the normal ASCII encoding has been
       replaced with UTF-8.

        Bool formatting
        ---------------

        When a bool value is written to the stream then it is printed as a word
       by default. It prints "true" or "false" (or the locale's translation of
       these, if a custom locale was set with imbue()). This is in contrast to
       the default for other std::basic_ostream implementations, which print
       bools as numbers (0 or 1) by default.

        Note that you can change this setting and switch to printing 0 and 1
       instead by doing this: \code stream << std::noboolalpha; \endcode


        Supported string types
        ----------------------

        TextOutStream supports strings of all standard character types: char,
       wchar_t, char16_t and char32_t. In addition to the Boden String class
       (see bdn::String), both zero terminated C-style strings (e.g. const
       char*) and std::basic_string objects (like std::string, std::wstring) are
       supported.

        All strings that use the "char" type are interpreted according to the
       encoding of the stream's locale (which is UTF-8 by default).


        Adding support for custom object types
        -----------------------------

        To enable objects of your custom types to be used with TextOutStream,
       you need to provide a
        << operator overload like this:

        \code

        // this is an example for a custom class
        class ShoppingItem
        {
        public:
            String	productName() const;
            int		amount() const;
        };

        // Note that this overload not only adds support for pushing
       ShoppingItem objects into
        // TextOutStream, but also for pushing them into any other standard I/O
       stream (like std::cout).

        template< typename CHAR, class TRAITS >
        std::basic_ostream<CHAR,TRAITS>& operator<<(
       std::basic_ostream<CHAR,TRAITS>& stream, const ShoppingItem& item)
        {
            // generate a string representation of the object. This is an
       example
            // for the shopping list item.
            return stream << item.amount() << " " << item.productName();
        }

        \endcode

        Handling objects of unsupported type
        ------------------------------------

        If the << operator is not overloaded for a given object type then
       "myStream << myObject" will cause a compiler error. However, sometimes is
       is useful to be able to "stringify" any object. For such cases the global
       function bdn::writeAnyToStream() exists. It can be used to write ANY
       object of any type to a stream like TextOutStream. If there is not <<
       operator for the object type then this function writes a default string
       and does not cause a compiler error.


        Technical background information
        --------------------

        See \ref text_streams.md for information on the technical implementation
       of these streams. Most users of the Boden framework do not need to worry
       about this, though.

    */
    using TextOutStream = std::basic_ostream<char32_t, UnicodeCharTraits>;

    // the following is a workaround for a Visual Studio bug that was fixed in
    // one of the updates of Visual Studio 2017. At the time of this writing it
    // is unclear which particular update fixed this - early versions of VS2017
    // had the bug and VS2017 v15.8 (_MSC_VER=1915) did not have the bug
    // anymore. So for the time being we set the cutoff for the workaround at
    // v15.7 (_MSC_VER 1914)
#if defined(_MSC_VER) && _MSC_VER <= 1914

    // This is a TextOutStream version of the normal << overload for
    // std::basic_ostream rvalue references. This overload ensures that the <<
    // operator also works with ad hoc created TextOutStream objects. Note that
    // this is only necessary with some compilers / standard libraries because
    // their conversion operator for this is sometimes buggy or not generic
    // enough. For example, for MSVC the standard library conversion operator is
    // only there for std::ostream, not for basic_ostream object of other char
    // or traits types.
    template <typename VALUE_TYPE>
    TextOutStream &operator<<(TextOutStream &&stream, VALUE_TYPE &&value)
    {
        return stream << std::forward<VALUE_TYPE>(value);
    }

#endif

    template <bool SUPPORTED> struct StreamWriterImpl_
    {
        template <typename STREAM_TYPE, typename VALUE_TYPE>
        static void write(STREAM_TYPE &&stream, VALUE_TYPE &&value)
        {
            std::forward<STREAM_TYPE>(stream)
                << std::forward<VALUE_TYPE>(value);
        }
    };

    template <> struct StreamWriterImpl_<false>
    {
        template <typename STREAM_TYPE, typename VALUE_TYPE>
        static void write(STREAM_TYPE &&stream, VALUE_TYPE &&value)
        {
            std::forward<STREAM_TYPE>(stream)
                << "<" << typeid(value).name() << " @ " << (const void *)&value
                << ">";
        }
    };

    /** Writes a string representation of any value or object (whatever its
       type) to the specified standard I/O stream. The stream can be any stream
       derived from or compatible with std::basic_ostream. This includes the
       Boden classes TextOutStream and StringBuffer.

        If a << operator overload exists for the specified value type and stream
        (i.e. if "stream << value" is supported) then the << operator will be
       used to write the value.

        If there is no << operator overload for the given value then a generic
       text consisting of the value type and its address is written instead.

        Returns a reference to the stream.
        */
    template <typename STREAM_TYPE, typename VALUE_TYPE>
    STREAM_TYPE &writeAnyToStream(STREAM_TYPE &&stream, VALUE_TYPE &&value)
    {
        StreamWriterImpl_<typeSupportsShiftLeftWith<
            typename std::decay<STREAM_TYPE>::type,
            decltype(value)>()>::write(stream, std::forward<VALUE_TYPE>(value));

        return stream;
    }
}

#endif
