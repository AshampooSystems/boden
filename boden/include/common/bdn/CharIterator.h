#ifndef _BDN_CharIterator_H_
#define _BDN_CharIterator_H_


#include <iterator>

namespace bdn
{
    
    template<class CharDecoderType, class EncodedIteratorType>
    class CharIterator : public std::iterator<UniChar, std::bidirectional_iterator_tag>
    {
    public:
        CharIterator(const EncodedIteratorType& encodedIt)
        {
            _encodedIt = encodedIt;
        }
        
        UniChar operator*() const
        {
            CharDecoderType::decodeChar(_encodedIt);
        }
        
        CharIterator<CharDecoderType,EncodedIteratorType> operator++()
        {
            _encodedIt = CharDecoderType::decodeChar(_encodedIt);
            return *this;
        }
        
        CharIterator<CharDecoderType,EncodedIteratorType> operator--()
        {
            _encodedIt = CodecType::decodePrecedingChar(_encodedIt);
            return *this;
        }
        
        bool operator==(const CharIterator<CharDecoderType,EncodedIteratorType>& o) const
        {
            return (_encodedIt==o._encodedIt);
        }
        
        bool operator!=(const CharIterator<CharDecoderType,EncodedIteratorType>& o) const
        {
            return (_encodedIt!=o._encodedIt);
        }
        
    protected:
        EncodedIteratorType _encodedIt;
    };
    
    
}


#endif