#pragma once

#include <locale>

namespace bdn
{

    /** Encodes string data to the multibyte encoding used in the given locale.

        LocaleEncoder follows the \ref sequence.md "sequence" protocol, i.e.
        it provides begin() and end() methods that return iterators. The
       iterators return the encoded char elements.

        The input data must be full 32 bit unicode characters. It is also
       provided via iterators. The template parameter of LocaleEncoder must be
       the type of the iterators that provide the source characters to encode.

        If a character cannot be represented in the locale's multibyte encoding
        then the encoder will try to encode the unicode replacement character
       instead (U'fffd'). If that character also cannot be encoded then the '?'
       character is encoded instead.

    */
    template <class SOURCE_ITERATOR_TYPE> class LocaleEncoder
    {
      public:
        /** \param sourceBeginIt an iterator that points to the first character
           to encode \param sourceEndIt an iterator that points to the position
           just after the last character to encode \param loc the locale whose
           multibyte encoding should be used to encode the data.*/
        LocaleEncoder(SOURCE_ITERATOR_TYPE &&sourceBeginIt, SOURCE_ITERATOR_TYPE &&sourceEndIt, const std::locale &loc)

            : _sourceBeginIt(std::forward<SOURCE_ITERATOR_TYPE>(sourceBeginIt)),
              _sourceEndIt(std::forward<SOURCE_ITERATOR_TYPE>(sourceEndIt))
              // note that we store a copy of the locale to ensure that it is
              // not deleted while we decode.
              ,
              _locale(loc)
        {
            _codec = &std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(_locale);
        }

#ifdef BDN_OVERRIDE_LOCALE_ENCODING_UTF8

        // we ignore the actual codec and ALWAYS use UTF-8.
        // This typedef is set when the standard library codec implementations
        // are so broken that they are unusable and the platform only supports
        // UTF-8 locales.

        using Iterator = Utf8Codec::EncodingIterator<SOURCE_ITERATOR_TYPE>;

        Iterator begin() const { return Iterator(_sourceBeginIt); }

        Iterator end() const { return Iterator(_sourceEndIt); }

#else

        class Iterator
        {
          public:
            // If we follow the standard precisely then we would need to declare
            // this as an input_iterator instead of a forward_iterator. The
            // reason is that we return values, not references, which only an
            // input iterator may do. However, input iterators are also single
            // pass only, so their use is very restrictive. String algorithms
            // frequently want to do multiple passes over the data (for example,
            // one to determine the length and one to do the actual work). We do
            // support multipass, so we declare ourselves as a forward
            // iterators, so that this capability will actually be used. In
            // practice it almost never causes a problem if a forward iterator
            // returns values instead of references.
            using iterator_category = std::forward_iterator_tag;
            using value_type = char;
            using difference_type = size_t;
            using pointer = char *;
            using reference = char;

            Iterator() : _codec(nullptr), _encodeState(nullptr) {}

          private:
            Iterator(const SOURCE_ITERATOR_TYPE &sourceBegin, const SOURCE_ITERATOR_TYPE &sourceEnd,
                     const std::codecvt<wchar_t, char, mbstate_t> *codec)
                : _wideNext(sourceBegin), _wideEnd(sourceEnd), _codec(codec), _encodeState(nullptr)
            {}

          public:
            Iterator(Iterator &&o)
                : _wideNext(std::move(o._wideNext)), _wideEnd(std::move(o._wideEnd)), _codec(o._codec),
                  _encodeState(o._encodeState)
            {
                o._encodeState = nullptr;
            }

            Iterator(const Iterator &o) : _wideNext(o._wideNext), _wideEnd(o._wideEnd), _codec(o._codec)
            {
                if (o._encodeState != nullptr)
                    _encodeState = new EncodeState(*o._encodeState);
                else
                    _encodeState = nullptr;
            }

            ~Iterator()
            {
                if (_encodeState != nullptr)
                    delete _encodeState;
            }

            Iterator &operator++()
            {
                if (_encodeState == nullptr) {
                    // this happens when we are at the start position and the
                    // first character has not been accessed. fill the output
                    // buffer first.
                    fillOutBuffer();
                }

                // _encodeState can still be null if the input data was empty.
                // In that case this call was invalid (++ past end of the data)
                if (_encodeState != nullptr) {
                    _encodeState->outPos++;

                    if (_encodeState->outPos >= _encodeState->outAvailable) {
                        // end of output data reached. Fill the buffer again
                        fillOutBuffer();

                        // if there is no more data to be decoded then this was
                        // an invalid call. (advancing past the end of the
                        // data).
                    }
                }

                return *this;
            }

            Iterator operator++(int)
            {
                Iterator oldVal = *this;
                operator++();

                return oldVal;
            }

            char operator*() const
            {
                if (_encodeState == nullptr)
                    fillOutBuffer();

                return _encodeState->outBuffer[_encodeState->outPos];
            }

            bool operator==(const Iterator &o) const
            {
                if (_encodeState == nullptr)
                    fillOutBuffer();
                if (o._encodeState == nullptr)
                    o.fillOutBuffer();

                // _encodeState is null if this is an end iterator
                // and also if it is an iterator that has reached the end of
                // the data.

                if (_wideNext == o._wideNext) {
                    // we must ensure that iterators that are on their last
                    // character do not match iterators that have reached the
                    // end. For both of these types, outPos may be 0 and
                    // wideNext points to the end of the sequence. The key is to
                    // compare the number of unreturned bytes in the output
                    // buffer.
                    int left = (_encodeState == nullptr) ? 0 : (_encodeState->outAvailable - _encodeState->outPos);
                    int otherLeft =
                        (o._encodeState == nullptr) ? 0 : (o._encodeState->outAvailable - o._encodeState->outPos);

                    return (left == otherLeft);
                } else
                    return false;
            }

            bool operator!=(const Iterator &o) const { return !operator==(o); }

            Iterator &operator=(Iterator &&o)
            {
                if (_encodeState != nullptr)
                    delete _encodeState;

                _wideNext = std::move(o._wideNext);
                _wideEnd = std::move(o._wideEnd);
                _codec = o._codec;
                _encodeState = o._encodeState;

                o._codec = nullptr;
                o._encodeState = nullptr;

                return *this;
            }

            Iterator &operator=(const Iterator &o)
            {
                if (_encodeState != nullptr)
                    delete _encodeState;

                _wideNext = o._wideNext;
                _wideEnd = o._wideEnd;
                _codec = o._codec;

                if (o._encodeState == nullptr)
                    _encodeState = nullptr;
                else
                    _encodeState = new EncodeState(*o._encodeState);

                return *this;
            }

          private:
            template <class Codec, class InType, class OutType>
            inline static int _callCodecOutAndCorrectBuggyImplementations(Codec &codec, mbstate_t &state,
                                                                          const InType *inBegin, const InType *inEnd,
                                                                          const InType *&inNext, OutType *outBegin,
                                                                          OutType *outEnd, OutType *&outNext)
            {
                int result = codec.out(state, inBegin, inEnd, inNext, outBegin, outEnd, outNext);

                // some buggy codec implementations will return stop when they
                // encounter a '\0' character and return ok. But they will not
                // consume the zero character and will not write it to output.
                if (result == std::codecvt_base::ok) {
                    if (inBegin != inEnd && outBegin != outEnd && inNext == inBegin && outNext == outBegin) {
                        // just copy the input character over. It is most likely
                        // a zero character.
                        *outNext = (char)*inNext;
                        outNext++;
                        inNext++;
                    }
                } else if (result == std::codecvt_base::partial) {
                    // some codec implementations (incorrectly) return "partial"
                    // when they encounter an error. If zero input and output
                    // was consumed then we can be certain that this is an
                    // "error partial" case. But if there were successfully
                    // converted characters before that then we cannot easily
                    // distinguish the error case from the normal partial result
                    // (end of buffer reached). But that does not matter: we can
                    // treat these cases like a normal partial result initially
                    // and then, in the next iteration, we will get a partial
                    // result again, this time with zero input and output
                    // consumed. So we will diagnose the error then.
                    if (inNext == inBegin && outNext == outBegin && inBegin != inEnd && outBegin != outEnd) {
                        // this also sometimes happens when the character is a
                        // zero character '\0'
                        if (inNext != inEnd && *inNext == 0) {
                            // the error character is the zero char. Just output
                            // it and continue.
                            *outNext = 0;
                            outNext++;
                            inNext++;
                            result = std::codecvt_base::ok;
                        } else {
                            // this is an error case. Correct the result.
                            result = std::codecvt_base::error;
                        }
                    }
                }

                return result;
            }

            /** Fills the output buffer. Returns false when that is not possible
               because the end of the input data has been reached.*/
            bool fillOutBuffer() const
            {
                if (_wideNext != _wideEnd) {
                    if (_encodeState == nullptr)
                        _encodeState = new EncodeState();

                    _encodeState->outAvailable = 0;
                    _encodeState->outPos = 0;

                    wchar_t wideBuffer[WideCodec::getMaxEncodedElementsPerCharacter()];

                    do {
                        // Copy one character to the wide buffer
                        bool characterEnd = false;
                        wchar_t *inEnd = wideBuffer;
                        do {
                            *inEnd = *_wideNext;

                            characterEnd = _wideNext.isEndOfCharacter();

                            ++inEnd;
                            ++_wideNext;
                        } while (!characterEnd);

                        const wchar_t *inNext = wideBuffer;
                        char *outNext = _encodeState->outBuffer;
                        char *outEnd = _encodeState->outBuffer + outBufferSize;

                        int convResult = _callCodecOutAndCorrectBuggyImplementations(
                            *_codec, _encodeState->state, wideBuffer, inEnd, inNext, _encodeState->outBuffer, outEnd,
                            outNext);

                        if (convResult == std::codecvt_base::error) {
                            // a character cannot be converted. The standard
                            // defines that inNext SHOULD point to that
                            // character. And all others up to that point should
                            // have been converted.

                            // But unfortunately with some standard libraries
                            // (e.g. on Mac with libc++) inNext and outNext
                            // always point to the first character, even if it
                            // is not the problem.

                            // Luckily, this is not a problem in this case,
                            // since we only convert a single character at the
                            // time. So we know that the first and only
                            // character is the problem.

                            // Insert a replacement character.
                            const wchar_t *replacement = L"\xfffd";
                            inNext = replacement;
                            outNext = _encodeState->outBuffer;

                            convResult = _callCodecOutAndCorrectBuggyImplementations(
                                *_codec, _encodeState->state, replacement, replacement + 1, inNext,
                                _encodeState->outBuffer, outEnd, outNext);
                            if (convResult != std::codecvt_base::ok) {
                                // character cannot be represented. Use question
                                // mark instead.
                                replacement = L"?";
                                inNext = replacement;
                                outNext = _encodeState->outBuffer;

                                convResult = _callCodecOutAndCorrectBuggyImplementations(
                                    *_codec, _encodeState->state, replacement, replacement + 1, inNext,
                                    _encodeState->outBuffer, outEnd, outNext);

                                // ignore the final replacement conversion
                                // result. If the ? character can also not be
                                // represented then we just insert nothing.
                            }
                        }

                        // note: outAvailable may still be 0 here if there was
                        // an encoding error and none of the replacement
                        // characters could be encoded either. In that case we
                        // try again with the next character.
                        _encodeState->outAvailable = (int)(outNext - _encodeState->outBuffer);
                        if (_encodeState->outAvailable > 0)
                            return true;
                    } while (_wideNext != _wideEnd);

                    // we reached the end. Delete the encode state
                    // here - it makes the == operator simpler if _encodeState
                    // is always null when we reached the end.
                    if (_encodeState != nullptr) {
                        delete _encodeState;
                        _encodeState = nullptr;
                    }
                }

                return false;
            }

            enum
            {
                outBufferSize = WideCodec::getMaxEncodedElementsPerCharacter() * MB_LEN_MAX * 2
            };

            struct EncodeState
            {
                EncodeState() : state(std::mbstate_t()), outAvailable(0), outPos(0) {}

                EncodeState(const EncodeState &o) : state(o.state), outAvailable(o.outAvailable), outPos(o.outPos)
                {
                    std::memcpy(outBuffer, o.outBuffer, sizeof(char) * o.outAvailable);
                    outAvailable = o.outAvailable;
                    outPos = o.outPos;
                }

                std::mbstate_t state;

                char outBuffer[outBufferSize + 1];
                int outAvailable;
                int outPos;
            };

            mutable WideCodec::EncodingIterator<SOURCE_ITERATOR_TYPE> _wideNext;
            WideCodec::EncodingIterator<SOURCE_ITERATOR_TYPE> _wideEnd;
            const std::codecvt<wchar_t, char, mbstate_t> *_codec;
            mutable EncodeState *_encodeState;

            friend class LocaleEncoder;
        };

        Iterator begin() const { return Iterator(_sourceBeginIt, _sourceEndIt, _codec); }

        Iterator end() const { return Iterator(_sourceEndIt, _sourceEndIt, _codec); }

#endif

      private:
        SOURCE_ITERATOR_TYPE _sourceBeginIt;
        SOURCE_ITERATOR_TYPE _sourceEndIt;
        const std::codecvt<wchar_t, char, mbstate_t> *_codec;
        std::locale _locale;
    };
}
