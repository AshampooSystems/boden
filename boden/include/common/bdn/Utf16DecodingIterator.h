#ifndef _BDN_Utf16DecodingIterator_H_
#define _BDN_Utf16DecodingIterator_H_

#include <iterator>

namespace bdn
{
    
    template<class SourceIterator>
    class Utf16DecodingIterator : public std::iterator<char, std::bdirectional_iterator_tag>
    {
    public:
        Utf16DecodingIterator(const SourceIterator& sourceIt, const SourceIterator& beginSourceIt, const SourceIterator& endSourceIt)
        {
            _sourceIt = sourceIt;
            _beginSourceIt = beginSourceIt;
            _endSourceIt = endSourceIt;
            
            _currChr = (UniChar)-1;
        }
        
        
        Utf16DecodingIterator<SourceIterator>& operator++()
        {
            if(_chr==(UniChar)-1)
            {
                // we still need to decode the current character (the
                // one at the position we are at right now). We need to
                // do that so that _nextIt will be valid.
                decode();
            }

            _sourceIt = _nextIt;
            
            return *this;
        }
        
        Utf16DecodingIterator<SourceIterator>& operator--()
        {
            _nextIt = _sourceIt;
            
            // iterate backwards until we find the beginning of
            // a sequence.
            _sourceIt--;
            _chr = *sourceIt;
            if(_chr>=0xdc00 && _chr<=0xdfff)
            {
                // a trailing surrogate. Need one more value.
                if(_sourceIt==_beginSourceIt)
                {
                    // invalid sequence at the start. Cannot go further back.
                    _chr = 0xfffd;
                }
                else
                {
                    _sourceIt--;
                    UniChar highVal = *_sourceIt;
                    if(val<0xd800 || val>0xdbff)
                    {
                        // invalid leading surrogate. Which means that the
                        // first one is also invalid.
                        _chr = 0xfffd;
                        _sourceIt++;
                    }
                    else
                    {
                        highVal -= 0xd800;
                        
                        _chr -= 0xdc00;
                        _chr |= highVal<<10;
                        _chr += 0x010000;
                    }
                }
            }
            
            return *this;
        }
        
        UniChar operator*()
        {
            if(_chr==(UniChar)-1)
                decode();
            
            return _currChr;
        }
        
        bool operator==(const Utf16DecodingIterator<SourceIterator>& o) const
        {
            return (_sourceIt!=o._sourceIt);
        }
        
        bool operator!=(const Utf16DecodingIterator<SourceIterator>& o) const
        {
            return !operator==(o);
        }
        
        
    protected:
        void decode()
        {
            _nextIt = _sourceIt;
            
            _chr = *_nextIt;
            _nextIt++;
            
            if(_chr>=0xd800 && _chr<=0xdbff)
            {
                // leading surrogate
                
                if(_nextIt==_endSourceIt)
                {
                    // no more data after the initial surrogate. Invalid value.
                    _chr = 0xfffd;
                }
                else
                {
                    UniChar lowVal = *_nextIt;
                    if(lowVal>=0xdc00 && lowVal<=0xdfff)
                    {
                        // valid trailing surrogate
                        lowVal -= 0xdc00;
                        _chr-=0xd800;
                        
                        _chr<<=10;
                        _chr |= lowVal;
                        
                        _chr += 0x010000;
                    }
                    else
                    {
                        // not a valid trailing surrogate. So the leading surrogate should
                        // be treated as an invalid value.
                        _chr = 0xfffd;
                        _nextIt--;
                    }
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


