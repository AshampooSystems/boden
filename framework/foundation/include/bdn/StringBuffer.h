#ifndef BDN_StringBuffer_H_
#define BDN_StringBuffer_H_

#include <bdn/TextOutStream.h>
#include <bdn/StreamFormat.h>
#include <bdn/List.h>

namespace bdn
{

    /** Provides an efficient way to construct strings from individual
        parts. StringBuffer is derived from \ref TextOutStream (which is a
       std::basic_ostream implementation). So StringBuffer can be used in the
       same way as C++ standard library output streams (like std::cout).

        Some important properties of StringBuffer:

        - It offers proper Unicode support. All formatting (like padding etc.)
          refers to full 32 bit Unicode characters.
        - Formatting is locale independent by default, using the formatting
       rules from the classic locale (english formatting).
        - Strings based on any character type (char, wchar_t, char16_t,
       char32_t) can be written to the buffer. The string data is properly
       decoded.
        - Multibyte strings (std::string, const char*) are interpreted as UTF-8
       by default. You can change that by setting a different locale with a
       different multibyte encoding (see imbue() ).
        - When parts of string arguments cannot be decoded because of corrupted
       data then the corresponding parts of the string are silently replaced
       with the unicode replacement character (U+fffd). So StringBuffer can
       safely be used with any kind of input data.


        Usage
        -----

        Like and standard C++ stream, you can write values and objects using the
       << operator. It is recommended to use \ref StreamFormat objects to
       configure the formatting of the individual values (although the normal
       C++ stream formatting works as well).

        Example:

        \code

        StringBuffer buffer;

        buffer << "The number is " << 42 << ". The size is " << Size(123, 456)
       << "." << std::endl; buffer << "Zero padding looks like this: " <<
       StreamFormat.zeroPad(4) << 17;

        String result = buffer.getString();

        // result will be:
        // The number is 42. The size is 123 x 456.
        // Zero padding looks like this: 0017

        \endcode

        */
    class StringBuffer : public Base, public TextOutStream
    {
      private:
        class StreamBuffer
            : public std::basic_streambuf<char32_t, UnicodeCharTraits>
        {
          public:
            StreamBuffer() : _totalCharsInFinishedChunks(0)
            {
                _writeChunkIt = _chunkList.end();
                _writeChunkStartPos = 0;

                _readChunkIt = _chunkList.end();
                _readChunkStartPos = 0;
            }

            // we do not currently implement a copy constructor (do not need it)
            StreamBuffer(const StreamBuffer &other) = delete;

            ~StreamBuffer() {}

            void swap(StreamBuffer &other)
            {
                syncChunkUsed();
                other.syncChunkUsed();

                std::basic_streambuf<char32_t, UnicodeCharTraits>::swap(other);

                // the end iterator of a list may get invalidated when it is
                // swapped. So we need to manually transfer those.
                bool otherWriteChunkItAtEnd =
                    (other._writeChunkIt == other._chunkList.end());
                bool writeChunkItAtEnd = (_writeChunkIt == _chunkList.end());
                bool otherReadChunkItAtEnd =
                    (other._readChunkIt == other._chunkList.end());
                bool readChunkItAtEnd = (_readChunkIt == _chunkList.end());

                std::swap(_chunkList, other._chunkList);
                std::swap(_writeChunkIt, other._writeChunkIt);
                std::swap(_writeChunkStartPos, other._writeChunkStartPos);

                std::swap(_totalCharsInFinishedChunks,
                          other._totalCharsInFinishedChunks);

                std::swap(_readChunkIt, other._readChunkIt);
                std::swap(_readChunkStartPos, other._readChunkStartPos);

                if (otherWriteChunkItAtEnd)
                    _writeChunkIt = _chunkList.end();
                if (writeChunkItAtEnd)
                    other._writeChunkIt = other._chunkList.end();

                if (otherReadChunkItAtEnd)
                    _readChunkIt = _chunkList.end();
                if (readChunkItAtEnd)
                    other._readChunkIt = other._chunkList.end();
            }

            size_t totalChars() const
            {
                syncChunkUsed();

                size_t result = _totalCharsInFinishedChunks;
                if (!_chunkList.isEmpty()) {
                    // the data in the last chunk is not included in
                    // _totalCharsInFinishedChunks
                    const Chunk &lastChunk = _chunkList.getLast();
                    result += lastChunk.used();
                }

                return result;
            }

            void advanceWritePosition(size_t chars) { pbump((int)chars); }

          private:
            class Chunk
            {
              public:
                Chunk() : _pData(nullptr), _capacity(0), _used(0) {}

                Chunk(size_t capacity) : _capacity(capacity), _used(0)
                {
                    _pData = new char32_t[capacity];
                }

                Chunk(Chunk &&other)
                    : _pData(other._pData), _capacity(other._capacity),
                      _used(other._used)
                {
                    other._pData = nullptr;
                    other._capacity = 0;
                    other._used = 0;
                }

                ~Chunk()
                {
                    if (_pData != nullptr)
                        delete[] _pData;
                }

                char32_t *data() { return _pData; }

                const char32_t *data() const { return _pData; }

                size_t capacity() const { return _capacity; }

                void setUsed(size_t used) { _used = used; }

                size_t used() const { return _used; }

              private:
                char32_t *_pData;
                size_t _capacity;
                size_t _used;
            };

          public:
            template <typename CHAR32_TYPE, typename CHUNK_IT_TYPE>
            class IteratorImpl_
            {
              public:
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type = char32_t; // not CHAR32_TYPE because we never
                                             // want this to be const
                using difference_type = size_t;
                using pointer = CHAR32_TYPE *;
                using reference = CHAR32_TYPE &;

                IteratorImpl_() : _indexInChunk(0) {}

                IteratorImpl_(const CHUNK_IT_TYPE &chunkIt, size_t indexInChunk)
                    : _chunkIt(chunkIt), _indexInChunk(indexInChunk)
                {}

                CHAR32_TYPE &operator*()
                {
                    return _chunkIt->data()[_indexInChunk];
                }

                const char32_t &operator*() const
                {
                    return _chunkIt->data()[_indexInChunk];
                }

                // accept other iterator types so that const and non-const
                // iterators can be compared with each other
                template <typename OTHER_CHAR32_TYPE,
                          typename OTHER_CHUNK_IT_TYPE>
                bool operator==(
                    const IteratorImpl_<OTHER_CHAR32_TYPE, OTHER_CHUNK_IT_TYPE>
                        &other) const
                {
                    return (_chunkIt == other._chunkIt &&
                            _indexInChunk == other._indexInChunk);
                }

                template <typename OTHER_CHAR32_TYPE,
                          typename OTHER_CHUNK_IT_TYPE>
                bool operator!=(
                    const IteratorImpl_<OTHER_CHAR32_TYPE, OTHER_CHUNK_IT_TYPE>
                        &other) const
                {
                    return !operator==(other);
                }

                IteratorImpl_ &operator++()
                {
                    size_t currChunkUsed = _chunkIt->used();

                    _indexInChunk++;

                    // note that we cannot use "while" here because
                    // at the end of the buffer _chunkIt will be an end-iterator
                    // and must not be dereferenced
                    if (_indexInChunk >= currChunkUsed) {
                        _indexInChunk -= currChunkUsed;
                        ++_chunkIt;
                    }

                    return *this;
                }

                IteratorImpl_ operator++(int)
                {
                    IteratorImpl_ copy(*this);
                    operator++();
                    return copy;
                }

                IteratorImpl_ &operator--()
                {
                    while (_indexInChunk == 0) {
                        --_chunkIt;
                        _indexInChunk = _chunkIt->used();
                    }

                    _indexInChunk--;

                    return *this;
                }

                IteratorImpl_ operator--(int)
                {
                    IteratorImpl_ copy(*this);
                    operator--();
                    return copy;
                }

              private:
                CHUNK_IT_TYPE _chunkIt;
                size_t _indexInChunk;
            };

            using Iterator =
                IteratorImpl_<char32_t, typename List<Chunk>::Iterator>;
            using ConstIterator =
                IteratorImpl_<const char32_t,
                              typename List<Chunk>::ConstIterator>;

            Iterator begin()
            {
                // note that _chunkList will be empty until the first
                // character is written. So begin() on an empty StreamBuffer
                // will return the same iterator as end(), which is what we
                // want.

                syncChunkUsed();

                return Iterator(_chunkList.begin(), 0);
            }

            Iterator end()
            {
                syncChunkUsed();

                return Iterator(_chunkList.end(), 0);
            }

            ConstIterator begin() const
            {
                syncChunkUsed();

                return ConstIterator(_chunkList.begin(), 0);
            }

            ConstIterator end() const
            {
                syncChunkUsed();

                return ConstIterator(_chunkList.end(), 0);
            }

          protected:
            int_type overflow(int_type chr) override
            {
                syncChunkUsed();

                if (chr == EOF) {
                    // end of data reached.
                    // nothing to do
                } else {
                    _nextWriteChunk();
                    sputc(chr);
                }

                return 0;
            }

            int sync() override
            {
                syncChunkUsed();

                return 0;
            }

            int_type underflow() override
            {
                size_t chunkAlreadyRead;

                syncChunkUsed();

                if (_readChunkIt == _chunkList.end()) {
                    // this is the first data being read.
                    if (_chunkList.isEmpty())
                        return traits_type::eof();

                    _readChunkIt = _chunkList.begin();
                    _readChunkStartPos = 0;

                    chunkAlreadyRead = 0;
                } else
                    chunkAlreadyRead = gptr() - eback();

                Chunk *pChunk = &*_readChunkIt;

                if (pChunk->used() <= chunkAlreadyRead) {
                    // advance to next chunk (if we have one).
                    ++_readChunkIt;

                    if (_readChunkIt == _chunkList.end()) {
                        // no more chunks. Set the iterator back to the last
                        // chunk, since more data may be added to that later.
                        --_readChunkIt;
                        return traits_type::eof();
                    }

                    _readChunkStartPos += pChunk->used();

                    // note that we never have empty chunks. So we know that
                    // there is data in the new chunk.
                    pChunk = &*_readChunkIt;
                    chunkAlreadyRead = 0;
                }

                char32_t *pChunkData = pChunk->data();

                setg(pChunkData, pChunkData + chunkAlreadyRead,
                     pChunkData + pChunk->used());

                return traits_type::to_int_type(pChunkData[chunkAlreadyRead]);
            }

            pos_type seekpos(pos_type pos,
                             ios_base::openmode which = ios_base::in |
                                                        ios_base::out) override
            {
                return seekoff(pos, ios_base::beg, which);
            }

          private:
            bool _moveToPosition(List<Chunk>::Iterator &chunkIt,
                                 size_t &posInChunk,
                                 std::streamoff &chunkStartPos, off_type offset,
                                 std::ios_base::seekdir anchor)
            {
                if (anchor == ios_base::end) {
                    chunkIt = _chunkList.end();
                    --chunkIt;

                    posInChunk = chunkIt->used();

                    chunkStartPos = _totalCharsInFinishedChunks;
                } else if (anchor == ios_base::beg) {
                    chunkIt = _chunkList.begin();

                    chunkStartPos = 0;
                    posInChunk = 0;
                } else {
                    // anchor is "cur". Keep the current values
                    // for chunkIt and posInChunk.
                    // Nothing else to do.
                }

                while (offset != 0) {
                    Chunk &chunk = *chunkIt;

                    if (offset > 0) {
                        size_t avail = chunk.used() - posInChunk;

                        if ((size_t)offset > avail) {
                            offset -= avail;
                            ++chunkIt;

                            if (chunkIt == _chunkList.end()) {
                                --chunkIt;
                                posInChunk = chunk.used();
                                return false;
                            }

                            chunkStartPos += chunk.used();

                            posInChunk = 0;
                        } else {
                            posInChunk = (size_t)(posInChunk + offset);
                            offset = 0;
                        }
                    } else {
                        if (((size_t)-offset) > posInChunk) {
                            offset += posInChunk;

                            if (chunkIt == _chunkList.begin()) {
                                posInChunk = 0;
                                return false;
                            }

                            chunkStartPos -= chunk.used();

                            --chunkIt;
                            posInChunk = chunkIt->used();
                        } else {
                            posInChunk = (size_t)(posInChunk + offset);
                            offset = 0;
                        }
                    }
                }

                return true;
            }

          protected:
            pos_type
            seekoff(off_type off, std::ios_base::seekdir anchor,
                    std::ios_base::openmode which = ios_base::in |
                                                    ios_base::out) override
            {
                syncChunkUsed();

                if (anchor == ios_base::cur &&
                    ((which & (ios_base::in | ios_base::out)) ==
                     (ios_base::in | ios_base::out))) {
                    // if both positions are modified at the same time then cur
                    // cannot be used, since the return value is not defined. We
                    // fail here, just like the normal std::basic_stringbuf.
                    return pos_type(off_type(-1));
                }

                if (_chunkList.isEmpty()) {
                    if (off != 0)
                        return pos_type(off_type(-1));
                    else
                        return pos_type(off_type(0));
                }

                // chunkList is not empty

                std::streamoff result = -1;

                if ((which & ios_base::in) == ios_base::in) {
                    size_t posInChunk = 0;

                    if (_readChunkIt == _chunkList.end()) {
                        // no data was read yet. So position is 0
                        _readChunkIt = _chunkList.begin();
                        posInChunk = 0;
                    } else
                        posInChunk = gptr() - eback();

                    if (_moveToPosition(_readChunkIt, posInChunk,
                                        _readChunkStartPos, off, anchor))
                        result = _readChunkStartPos + posInChunk;

                    if (_readChunkIt == _chunkList.end())
                        setg(nullptr, nullptr, nullptr);
                    else {
                        Chunk &chunk = *_readChunkIt;
                        setg(chunk.data(), chunk.data() + posInChunk,
                             chunk.data() + chunk.used());
                    }
                }

                if ((which & ios_base::out) == ios_base::out) {
                    size_t posInChunk = getPosInChunk();

                    if (_moveToPosition(_writeChunkIt, posInChunk,
                                        _writeChunkStartPos, off, anchor))
                        result = _writeChunkStartPos + posInChunk;

                    Chunk &chunk = *_writeChunkIt;
                    setp(chunk.data(), chunk.data() + chunk.capacity());
                    pbump((int)posInChunk);
                }

                return result;
            }

          private:
            size_t getPosInChunk() const
            {
                char32_t *pBase = pbase();
                char32_t *pCurr = pptr();
                if (pBase != nullptr && pCurr != nullptr)
                    return pCurr - pBase;
                else
                    return 0;
            }

            void syncChunkUsed() const
            {
                if (_writeChunkIt != _chunkList.end()) {
                    Chunk &chunk = *_writeChunkIt;

                    size_t posInChunk = getPosInChunk();
                    if (posInChunk > chunk.used()) {
                        chunk.setUsed(posInChunk);

                        if (_readChunkIt == _writeChunkIt) {
                            // update the end of the read buffer, since the
                            // amount of valid data in this chunk has changed.
                            const_cast<StreamBuffer *>(this)->setg(
                                chunk.data(), gptr(),
                                chunk.data() + chunk.used());
                        }
                    }
                }
            }

            void _nextWriteChunk()
            {
                syncChunkUsed();

                bool addNewChunk = false;

                if (_writeChunkIt == _chunkList.end())
                    addNewChunk = true;
                else {
                    _writeChunkStartPos += _writeChunkIt->used();

                    ++_writeChunkIt;
                    if (_writeChunkIt == _chunkList.end()) {
                        // the chunk we wrote in was the last
                        --_writeChunkIt;
                        _totalCharsInFinishedChunks += _writeChunkIt->used();

                        // add a new one
                        addNewChunk = true;
                    }
                }

                if (addNewChunk) {
                    // the chunk list is currently empty add a new chunk
                    _chunkList.addNew(defaultChunkCapacity);

                    _writeChunkIt = --_chunkList.end();
                }

                Chunk &chunk = *_writeChunkIt;

                char32_t *pData = chunk.data();

                setp(pData, pData + chunk.capacity());
            }

            enum
            {
                defaultChunkCapacity = 128
            };

            List<Chunk> _chunkList;

            // sum of number of characters in all the chunks except the last one
            size_t _totalCharsInFinishedChunks;

            List<Chunk>::Iterator _writeChunkIt;
            std::streamoff _writeChunkStartPos;

            List<Chunk>::Iterator _readChunkIt;
            std::streamoff _readChunkStartPos;
        };

      public:
        StringBuffer() { init(&_buffer); }

        StringBuffer(const StringBuffer &rhs) = delete;

        StringBuffer(StringBuffer &&rhs) : TextOutStream(std::move(rhs))
        {
            _buffer.swap(rhs._buffer);

            // we do not call init because the stream was initialized by swap.
            // But we must set the stream buffer pointer to our member object.
            set_rdbuf(&_buffer);

            // the move constructor does not touch the buffer pointer. So we
            // must manually remove the source object's buffer, since it may
            // have been invalidated by the move
            rhs.set_rdbuf(nullptr);
        }

        virtual ~StringBuffer() {}

        StringBuffer &operator=(const StringBuffer &rhs) = delete;

        StringBuffer &operator=(StringBuffer &&rhs)
        {
            TextOutStream::operator=(std::move(rhs));

            _buffer.swap(rhs._buffer);

            return *this;
        }

        using Iterator = StreamBuffer::Iterator;
        using ConstIterator = StreamBuffer::ConstIterator;

        Iterator begin() { return _buffer.begin(); }

        ConstIterator begin() const { return _buffer.begin(); }

        Iterator end() { return _buffer.end(); }

        ConstIterator end() const { return _buffer.end(); }

        /** Returns the contents of the StringBuffer as a normal String object.
            Note that a StringBuffer objects and the standard std::basic_ostream
            references that are often the result of << operations on a
           StringBuffer can be implicitly converted to Strings as well.

            So instead of calling toString explicitly, you can also simply
           assign the StringBuffer object or the result of a << chain directly
           to a String object.
            */
        String toString() const
        {
            // note that reserving space in the string is not necessary.
            // The iterators are bidirectional (i.e. they support multipass),
            // so the string implementation can
            // determine the number of characters with std::distance.
            return String(begin(), end());
        }

      private:
        StreamBuffer _buffer;
    };

    /** Returns a string representation of the specified value or object.

        The conversion is NOT locale dependent - the result is the same on
        all systems. Internally, the rules of the "C" locale are used - i.e.
       english number formatting.

        toString uses \ref StringBuffer internally, so it supports all types
        that have an overloaded << operator for \ref StringBuffer, \ref
       TextOutStream or \ref std::basic_ostream. See \ref TextOutStream for more
       inforation on how to add support for your own custom types.

        When you call toString with an object or a value of an unsupported type
       then you will get a compiler error. There is also an alternative function
       that will never give you a compiler error and that can be called with any
       type of value or object. See anyToString() for more information.
    */
    template <typename VALUE_TYPE> inline String toString(VALUE_TYPE &&value)
    {
        StringBuffer buf;
        buf << std::forward<VALUE_TYPE>(value);
        return buf;
    }

    /** Returns a string representation of the specified value or object (all
       types are supported).

        anyToString() is similar to the global function toString(), except that
        it works with values and objects of any type.

        Like toString(), anyToString internally uses \ref StringBuffer to
       generate the string representation of the value or object. If the
       specified value has a
        << operator for \ref StringBuffer, \ref TextOutStream or \ref
       std::basic_ostream then anyToString will use that to generate the string
       representation. See \ref TextOutStream for more inforation on how to add
       a string generator for your own type.

        When there is no usable << operator for the specified value type then
       calling anyToString does *not* cause a compiler error (in contrast to
       toString). Instead, anyToString will generate a string representation for
       such values automatically. The generated string representation contains
        the type name of the value, as returned by typeid(value).name(), as well
       as the value's memory address.

        */
    template <typename VALUE_TYPE> inline String anyToString(VALUE_TYPE &&value)
    {
        StringBuffer buf;
        writeAnyToStream(buf, std::forward<VALUE_TYPE>(value));
        return buf.toString();
    }
}

#endif
