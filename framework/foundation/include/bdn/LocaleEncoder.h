#ifndef BDN_LocaleEncoder_H_
#define BDN_LocaleEncoder_H_

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
        LocaleEncoder(SOURCE_ITERATOR_TYPE &&sourceBeginIt,
                      SOURCE_ITERATOR_TYPE &&sourceEndIt,
                      const std::locale &loc)

            : _sourceBeginIt(std::forward<SOURCE_ITERATOR_TYPE>(sourceBeginIt)),
              _sourceEndIt(std::forward<SOURCE_ITERATOR_TYPE>(sourceEndIt))
              // note that we store a copy of the locale to ensure that it is
              // not deleted while we decode.
              ,
              _locale(loc)
        {
            _pCodec = &std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(
                _locale);
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

            Iterator() : _pCodec(nullptr), _pEncodeState(nullptr) {}

          private:
            Iterator(const SOURCE_ITERATOR_TYPE &sourceBegin,
                     const SOURCE_ITERATOR_TYPE &sourceEnd,
                     const std::codecvt<wchar_t, char, mbstate_t> *pCodec)
                : _wideNext(sourceBegin), _wideEnd(sourceEnd), _pCodec(pCodec),
                  _pEncodeState(nullptr)
            {}

          public:
            Iterator(Iterator &&o)
                : _wideNext(std::move(o._wideNext)),
                  _wideEnd(std::move(o._wideEnd)), _pCodec(o._pCodec),
                  _pEncodeState(o._pEncodeState)
            {
                o._pEncodeState = nullptr;
            }

            Iterator(const Iterator &o)
                : _wideNext(o._wideNext), _wideEnd(o._wideEnd),
                  _pCodec(o._pCodec)
            {
                if (o._pEncodeState != nullptr)
                    _pEncodeState = new EncodeState(*o._pEncodeState);
                else
                    _pEncodeState = nullptr;
            }

            ~Iterator()
            {
                if (_pEncodeState != nullptr)
                    delete _pEncodeState;
            }

            Iterator &operator++()
            {
                if (_pEncodeState == nullptr) {
                    // this happens when we are at the start position and the
                    // first character has not been accessed. fill the output
                    // buffer first.
                    fillOutBuffer();
                }

                // _pEncodeState can still be null if the input data was empty.
                // In that case this call was invalid (++ past end of the data)
                if (_pEncodeState != nullptr) {
                    _pEncodeState->outPos++;

                    if (_pEncodeState->outPos >= _pEncodeState->outAvailable) {
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
                if (_pEncodeState == nullptr)
                    fillOutBuffer();

                return _pEncodeState->outBuffer[_pEncodeState->outPos];
            }

            bool operator==(const Iterator &o) const
            {
                if (_pEncodeState == nullptr)
                    fillOutBuffer();
                if (o._pEncodeState == nullptr)
                    o.fillOutBuffer();

                // _pEncodeState is null if this is an end iterator
                // and also if it is an iterator that has reached the end of
                // the data.

                if (_wideNext == o._wideNext) {
                    // we must ensure that iterators that are on their last
                    // character do not match iterators that have reached the
                    // end. For both of these types, outPos may be 0 and
                    // wideNext points to the end of the sequence. The key is to
                    // compare the number of unreturned bytes in the output
                    // buffer.
                    int left = (_pEncodeState == nullptr)
                                   ? 0
                                   : (_pEncodeState->outAvailable -
                                      _pEncodeState->outPos);
                    int otherLeft = (o._pEncodeState == nullptr)
                                        ? 0
                                        : (o._pEncodeState->outAvailable -
                                           o._pEncodeState->outPos);

                    return (left == otherLeft);
                } else
                    return false;
            }

            bool operator!=(const Iterator &o) const { return !operator==(o); }

            Iterator &operator=(Iterator &&o)
            {
                if (_pEncodeState != nullptr)
                    delete _pEncodeState;

                _wideNext = std::move(o._wideNext);
                _wideEnd = std::move(o._wideEnd);
                _pCodec = o._pCodec;
                _pEncodeState = o._pEncodeState;

                o._pCodec = nullptr;
                o._pEncodeState = nullptr;

                return *this;
            }

            Iterator &operator=(const Iterator &o)
            {
                if (_pEncodeState != nullptr)
                    delete _pEncodeState;

                _wideNext = o._wideNext;
                _wideEnd = o._wideEnd;
                _pCodec = o._pCodec;

                if (o._pEncodeState == nullptr)
                    _pEncodeState = nullptr;
                else
                    _pEncodeState = new EncodeState(*o._pEncodeState);

                return *this;
            }

          private:
            template <class Codec, class InType, class OutType>
            inline static int _callCodecOutAndCorrectBuggyImplementations(
                Codec &codec, mbstate_t &state, const InType *pInBegin,
                const InType *pInEnd, const InType *&pInNext,
                OutType *pOutBegin, OutType *pOutEnd, OutType *&pOutNext)
            {
                int result = codec.out(state, pInBegin, pInEnd, pInNext,
                                       pOutBegin, pOutEnd, pOutNext);

                // some buggy codec implementations will return stop when they
                // encounter a '\0' character and return ok. But they will not
                // consume the zero character and will not write it to output.
                if (result == std::codecvt_base::ok) {
                    if (pInBegin != pInEnd && pOutBegin != pOutEnd &&
                        pInNext == pInBegin && pOutNext == pOutBegin) {
                        // just copy the input character over. It is most likely
                        // a zero character.
                        *pOutNext = (char)*pInNext;
                        pOutNext++;
                        pInNext++;
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
                    if (pInNext == pInBegin && pOutNext == pOutBegin &&
                        pInBegin != pInEnd && pOutBegin != pOutEnd) {
                        // this also sometimes happens when the character is a
                        // zero character '\0'
                        if (pInNext != pInEnd && *pInNext == 0) {
                            // the error character is the zero char. Just output
                            // it and continue.
                            *pOutNext = 0;
                            pOutNext++;
                            pInNext++;
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
                    if (_pEncodeState == nullptr)
                        _pEncodeState = new EncodeState();

                    _pEncodeState->outAvailable = 0;
                    _pEncodeState->outPos = 0;

                    wchar_t wideBuffer
                        [WideCodec::getMaxEncodedElementsPerCharacter()];

                    do {
                        // Copy one character to the wide buffer
                        bool characterEnd = false;
                        wchar_t *pInEnd = wideBuffer;
                        do {
                            *pInEnd = *_wideNext;

                            characterEnd = _wideNext.isEndOfCharacter();

                            ++pInEnd;
                            ++_wideNext;
                        } while (!characterEnd);

                        const wchar_t *pInNext = wideBuffer;
                        char *pOutNext = _pEncodeState->outBuffer;
                        char *pOutEnd =
                            _pEncodeState->outBuffer + outBufferSize;

                        int convResult =
                            _callCodecOutAndCorrectBuggyImplementations(
                                *_pCodec, _pEncodeState->state, wideBuffer,
                                pInEnd, pInNext, _pEncodeState->outBuffer,
                                pOutEnd, pOutNext);

                        if (convResult == std::codecvt_base::error) {
                            // a character cannot be converted. The standard
                            // defines that pInNext SHOULD point to that
                            // character. And all others up to that point should
                            // have been converted.

                            // But unfortunately with some standard libraries
                            // (e.g. on Mac with libc++) pInNext and pOutNext
                            // always point to the first character, even if it
                            // is not the problem.

                            // Luckily, this is not a problem in this case,
                            // since we only convert a single character at the
                            // time. So we know that the first and only
                            // character is the problem.

                            // Insert a replacement character.
                            const wchar_t *pReplacement = L"\xfffd";
                            pInNext = pReplacement;
                            pOutNext = _pEncodeState->outBuffer;

                            convResult =
                                _callCodecOutAndCorrectBuggyImplementations(
                                    *_pCodec, _pEncodeState->state,
                                    pReplacement, pReplacement + 1, pInNext,
                                    _pEncodeState->outBuffer, pOutEnd,
                                    pOutNext);
                            if (convResult != std::codecvt_base::ok) {
                                // character cannot be represented. Use question
                                // mark instead.
                                pReplacement = L"?";
                                pInNext = pReplacement;
                                pOutNext = _pEncodeState->outBuffer;

                                convResult =
                                    _callCodecOutAndCorrectBuggyImplementations(
                                        *_pCodec, _pEncodeState->state,
                                        pReplacement, pReplacement + 1, pInNext,
                                        _pEncodeState->outBuffer, pOutEnd,
                                        pOutNext);

                                // ignore the final replacement conversion
                                // result. If the ? character can also not be
                                // represented then we just insert nothing.
                            }
                        }

                        // note: outAvailable may still be 0 here if there was
                        // an encoding error and none of the replacement
                        // characters could be encoded either. In that case we
                        // try again with the next character.
                        _pEncodeState->outAvailable =
                            (int)(pOutNext - _pEncodeState->outBuffer);
                        if (_pEncodeState->outAvailable > 0)
                            return true;
                    } while (_wideNext != _wideEnd);

                    // we reached the end. Delete the encode state
                    // here - it makes the == operator simpler if _pEncodeState
                    // is always null when we reached the end.
                    if (_pEncodeState != nullptr) {
                        delete _pEncodeState;
                        _pEncodeState = nullptr;
                    }
                }

                return false;
            }

            enum
            {
                outBufferSize = WideCodec::getMaxEncodedElementsPerCharacter() *
                                MB_LEN_MAX * 2
            };

            struct EncodeState
            {
                EncodeState()
                    : state(std::mbstate_t()), outAvailable(0), outPos(0)
                {}

                EncodeState(const EncodeState &o)
                    : state(o.state), outAvailable(o.outAvailable),
                      outPos(o.outPos)
                {
                    std::memcpy(outBuffer, o.outBuffer,
                                sizeof(char) * o.outAvailable);
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
            const std::codecvt<wchar_t, char, mbstate_t> *_pCodec;
            mutable EncodeState *_pEncodeState;

            friend class LocaleEncoder;
        };

        Iterator begin() const
        {
            return Iterator(_sourceBeginIt, _sourceEndIt, _pCodec);
        }

        Iterator end() const
        {
            return Iterator(_sourceEndIt, _sourceEndIt, _pCodec);
        }

#endif

      private:
        SOURCE_ITERATOR_TYPE _sourceBeginIt;
        SOURCE_ITERATOR_TYPE _sourceEndIt;
        const std::codecvt<wchar_t, char, mbstate_t> *_pCodec;
        std::locale _locale;
    };
}

#endif
