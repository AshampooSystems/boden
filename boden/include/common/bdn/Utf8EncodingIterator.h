#ifndef _BDN_Utf8EncodingIterator_H_
#define _BDN_Utf8EncodingIterator_H_

#include <iterator>

namespace bdn
{
    
    template<class SourceIterator>
    class Utf8EncodingIterator : public std::iterator<char, std::bdirectional_iterator_tag>
    {
    public:
        Utf8EncodingIterator(const SourceIterator& sourceIt)
        {
            _sourceIt = sourceIt;
            
            // we cannot call encode yet, because sourceIt might be
            // at an invalid position (i.e. end()). So instead we use an
            // invalid offset and encode the first time we dereference.
            _offset=-1;
        }
        
        
        Utf8EncodingIterator<SourceIterator>& operator++()
        {
            if(_offset==-1)
                encode();
            
            _offset++;
            if(offset>6)
            {
                _sourceIt++;
                _offset=-1;
            }
            
            return *this;
        }
        
        Utf8EncodingIterator<SourceIterator>& operator--()
        {
            if(_offset==-1 || _encoded[_offset-1]==0xff)
            {
                // _offset==-1 means that this is the starting position
                // of the iterator, i.e. the current position is the first byte of the first character..
                // _encoded[_offset-1]==0xff means that the current byte is the
                // first byte of the encoded sequence.
                
                _sourceIt--;
                _encode();
                
                // move to the last byte of the sequence. Note that that is always index 6
                // (the first byte offset changes depending on the sequence size - the end index stays the same).
                _offset = 6;
            }
            else
                _offset--;
            
            return *this;
        }
        
        char operator*()
        {
            if(_offset==-1)
                encode();
            
            return _encoded[_offset];
        }
        
        bool operator==(const Utf8EncodingIterator<SourceIterator>& o) const
        {
            if(_sourceIt!=o._sourceIt)
                return false;
            
            // 0 and -1 are the same position (-1 means "0 but still need to call encode")
            return (_offset==o._offset || (_offset<=0 && o._offset<=0));
        }
        
        bool operator!=(const Utf8EncodingIterator<SourceIterator>& o) const
        {
            return !operator==(o);
        }
        
        
    protected:
        void encode()
        {
            UniChar chr = *_sourceIt;
            
            if(chr<0 || chr>0x7FFFFFFF)
            {
                // invalid unicode character. Use 'replacement character'
                chr = 0xfffd;
            }
            
            int firstBytePayloadMask = 0x7f;
            
            _offset=6;
            while((chr & firstBytePayloadMask)!=0)
            {
                _encoded[offset] = 0x80 | (chr & 0x3f);
                
                chr>>=6;
                _offset--;
                
                firstBytePayloadMask >>= 1;
            }
            
            _encoded[_offset] = (~(firstBytePayloadMask<<1)) | chr;
            _encoded[_offset-1] = 0xff;
        }
        
        
        SourceIterator  _sourceIt;
        int             _offset = 0;
        uint8_t         _encoded[7];
            
    };
        
    
    
}


#endif


