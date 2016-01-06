#ifndef _BDN_Utf16EncodingIterator_H_
#define _BDN_Utf16EncodingIterator_H_

#include <iterator>

namespace bdn
{
    
    template<class SourceIterator>
    class Utf16EncodingIterator : public std::iterator<char, std::bdirectional_iterator_tag>
    {
    public:
        Utf16EncodingIterator(const SourceIterator& sourceIt)
        {
            _sourceIt = sourceIt;
            
            // we cannot call encode yet, because sourceIt might be
            // at an invalid position (i.e. end()). So instead we use an
            // invalid offset and encode the first time we dereference.
            _offset=-1;
        }
        
        
        Utf16EncodingIterator<SourceIterator>& operator++()
        {
            if(_offset==-1)
                encode();
            
            _offset++;
            if(_offset>=_encodedLength)
            {
                _sourceIt++;
                _offset=-1;
            }
            
            return *this;
        }
        
        Utf16EncodingIterator<SourceIterator>& operator--()
        {
            if(_offset<=0)
            {
                // _offset==-1 means that the current character has not yet been encoded. That also
                // means that we are at the beginning of the current character.
                
                _sourceIt--;
                _encode();
                
                // move to the last value of the sequence.
                _offset = _encodedLength-1;
            }
            else
                _offset--;
            
            return *this;
        }
        
        char16_t operator*()
        {
            if(_offset==-1)
                encode();
            
            return (char16_t)_encoded[_offset];
        }
        
        bool operator==(const Utf16EncodingIterator<SourceIterator>& o) const
        {
            if(_sourceIt!=o._sourceIt)
                return false;
            
            // 0 and -1 are the same position (-1 means "0 but still need to call encode")
            return (_offset==o._offset || (_offset<=0 && o._offset<=0));
        }
        
        bool operator!=(const Utf16EncodingIterator<SourceIterator>& o) const
        {
            return !operator==(o);
        }
        
        
    protected:
        void encode()
        {
            UniChar chr = *_sourceIt;
            
            if(chr<0 || chr>0x10ffff || (chr>=0xd800 && chr<=0xdfff))
            {
                // invalid, unencodable or in the surrogate range. Use 'replacement character'
                chr = 0xfffd;
            }
            
            if(chr>=0x10000)
            {
                // encode as surrogate pair
                chr -= 0x10000;
                _encoded[0] = (chr>>10) + 0xd800;
                _encoded[1] = (chr & 0x03ff) + 0xdc00;
                _encodedLength = 2;
            }
            else
            {
                _encoded[0] = chr;
                _encodedLength = 1;
            }
            
            _offset = 0;
        }
        
        
        SourceIterator  _sourceIt;
        int             _encoded[2];
        int             _encodedLength;
        int             _offset;
    };
    
    
    
}


#endif


