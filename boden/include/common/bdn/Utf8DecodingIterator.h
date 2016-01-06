#ifndef _BDN_Utf8DecodingIterator_H_
#define _BDN_Utf8DecodingIterator_H_

#include <iterator>

namespace bdn
{
    
    template<class SourceIterator>
    class Utf8DecodingIterator : public std::iterator<char, std::bdirectional_iterator_tag>
    {
    public:
        Utf8DecodingIterator(const SourceIterator& sourceIt, const SourceIterator& beginSourceIt, const SourceIterator& endSourceIt)
        {
            _sourceIt = sourceIt;
            _beginSourceIt = beginSourceIt;
            _endSourceIt = endSourceIt;
            
            _currChr = (UniChar)-1;
        }
        
        
        Utf8DecodingIterator<SourceIterator>& operator++()
        {
            if(_chr==(UniChar)-1)
                decode();
            _sourceIt = _nextIt;
            
            return *this;
        }
        
        Utf8DecodingIterator<SourceIterator>& operator--()
        {
            SourceIterator startIt = _sourceIt;
            
            // iterate backwards until we find the beginning of
            // a sequence.
            bool invalid = false;
            
            _sourceIt--;
            for(int i=0; ((*_sourceIt) & 0x0c)!=0x80; i++ )
            {
                if(_sourceIt==_beginSourceIt || i==5)
                {
                    // cannot advance further back. So we have an invalid
                    // sequence at the start of our data.
                    invalid = true;
                    break;
                }
                
                _sourceIt--;
            }
            
            if(!invalid)
            {
                decode();
            
                if(_nextIt!=startIt)
                    invalid = true;
            }
            
            if(invalid)
            {
                // the sequence was invalid and not all the bytes we skipped
                // over were consumed. When we decode the "next character" forwards from
                // here, the next bytes until the end of the invalid sequence
                // will be decoded as a single replacement character each.
                // Since we want decoding forwards and backwards to yield exactly the same
                // result we only go back a single byte.
                _nextIt = startIt;
                _sourceIt = startIt;
                _sourceIt--;
                
                _chr = 0xfffd;
            }
            
            return *this;
        }
        
        UniChar operator*()
        {
            if(_chr==(UniChar)-1)
                decode();
            
            return _currChr;
        }
        
        bool operator==(const Utf8EncodingIterator<SourceIterator>& o) const
        {
            return (_sourceIt!=o._sourceIt);
        }
        
        bool operator!=(const Utf8EncodingIterator<SourceIterator>& o) const
        {
            return !operator==(o);
        }
        
        
    protected:
        void decode()
        {
            _nextIt = _sourceIt;
            
            uint8_t firstByte = *_nextIt;
            
            _nextIt++;
            
            if((firstByte & 0x80)==0)
                _chr = firstByte;
            else
            {
                int     mask = 0x40;
                bool    invalid = false;
                int     length=1;
                
                _chr = 0;

                while(true)
                {
                    if((firstByte & mask)==0)
                    {
                        // end of sequence
                        break;
                    }
                    
                    if(_nextIt==_endSourceIt)
                    {
                        // we should have one more byte, but we don't.
                        invalid = true;
                        break;
                    }
                    
                    uint8_t val = *_nextIt;
                    if((val & 0xc0)!=0x80)
                    {
                        // invalid sequence. We should abort right away, because the current
                        // value could be part of a valid sequence that follows the broken one.
                        invalid = true;
                        break;
                    }
                    
                    chr <<= 6;
                    _chr |= (val & 0x3f);
                    
                    maskBit >>= 1;
                    
                    _nextIt++;
                    length++;
                }
                
                if(invalid || length==1)
                {
                    // use the unicode "replacement character".
                    _chr = 0xfffd;
                    // only consume a single byte.
                    _nextIt = sourceIt;
                    _nextIt++;
                }
                else
                {
                    _chr |= (((UniChar)firstByte) & (mask-1)) << (length*6);
                }
            }
        }
   
        
        SourceIterator  _sourceIt;
        SourceIterator  _beginSourceIt;
        SourceIterator  _endSourceIt;
        
        UniChar         _chr;
        SourceIterator  _nextIt;
        
    };
    
    
    
}


#endif


