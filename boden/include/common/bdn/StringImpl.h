#ifndef _BDN_StringImpl_H_
#define _BDN_StringImpl_H_

#include <bdn/Base.h>

namespace bdn
{
    
    
    template<class EncodedCharType, class Alloc=std::allocator<EncodedCharType> >
    class StringImpl : public Base
    {
        typedef basic_string<EncodedCharType, std::char_traits<EncodedCharType>, Alloc> EncodedStringType;
        typedef CharIterator<CodecType, EncodedStringType::iterator> Iterator;
        
    public:
        StringImpl(const char* s, int lengthElements=-1)
        : StringImpl( Utf8CharDecoder dec(s, lengthElements) )
        {
        }
        
        StringImpl(const std::string& s, int lengthElements=-1)
        : StringImpl( Utf8CharDecoder dec(s.c_str(), lengthElements) )
        {
        }
        
        StringImpl(const char16_t* s, int lengthElements=-1)
        : StringImpl( Utf16CharDecoder dec(s, lengthElements) )
        {
        }
        
        StringImpl(const std::wstring& s, int lengthElements=-1)
        : StringImpl( Utf16CharDecoder dec(s.c_str(), lengthElements) )
        {
        }
        
        StringImpl(const std::u16string& s, int lengthElements=-1)
        : StringImpl( Utf16CharDecoder dec(s.c_str(), lengthElements) )
        {
        }

        
        StringImpl(const char32_t* s, int lengthElements=-1)
        : StringImpl( Utf32CharDecoder dec(s, lengthElements) )
        {
        }
        
        StringImpl(const std::u32string& s, int lengthElements=-1)
        : StringImpl( Utf16CharDecoder dec(s.c_str(), lengthElements) )
        {
        }
        
        template<class CharCollection>
        StringImpl( const CharCollection& coll )
        : StringImpl( coll.begin(), coll.end() )
        {
        }
        
        template<class IteratorType>
        StringImpl( IteratorType beginIt, IteratorType endIt)
        {
            _charCount = 0;
            
            int encodedSizeWithoutTerminator = CharCodec::getEncodedSize(beginIt, endIt);
            
            if(encodedSizeWithoutTerminator==0)
                _pEncodedString = getEmptyEncodedString();
            else
            {
                _pEncodedString = make_shared<EncodedStringType>();
                _pEncodedString->reserve( encodedSizeWithoutTerminator + CharCodec::getEncodedSize(0) );
                
                for( IteratorType it=beginIt; it!=endIt; it++)
                {
                    CharCodec::encodePush(*it, *_pEncodedString);
                    _charCount++;
                }
            
                CharCodec::encodePush((UniChar)0, *_pEncodedString);
            }
        }
        
        int getCharCount() const
        {
            return _charCount;
        }
        
        bool isEmpty() const
        {
            return (_charCount==0);
        }
        
        const EncodedStringType& operator() const
        {
            return *_pEncodedString;
        }
        
        const EncodedStringType& toStd() const
        {
            return *_pEncodedString;
        }
        
        const EnodedCharType* c_str() const
        {
            return _pEncodedString->c_str();
        }
        
        
        
        Iterator begin() const
        {
            return Iterator(_pEncodedString->begin());
        }
        
        Iterator end() const
        {
            return Iterator(_pEncodedString->end());
        }
        
        
        UniChar front() const
        {
            return *begin();
        }
        
        UniChar back() const
        {
            Iterator endIt(_pEncodedString->end());
            endIt--;
            return *endIt;
        }
        
        
        Iterator find(UniChar chr) const
        {
            return std::find(begin(), end(), chr);
        }
        
        Iterator find(UniChar chr, const Iterator& beginIt) const
        {
            return std::find(beginIt, end(), chr);
        }
        
        Iterator find(UniChar chr, const Iterator& beginIt) const
        {
            return std::find(beginIt, end(), chr);
        }
        
        
        template<class ToFindIterator>
        Iterator find(const ToFindIterator& toFindBegin, const ToFindIterator& toFindEnd, Iterator* pMatchEnd=nullptr) const
        {
            for(Iterator matchBegin = begin(); matchBegin!=end(); matchBegin++)
            {
                ToFindIterator  toFindCurr = toFindBegin;
                Iterator        matchCurr = matchBegin;
                while(true)
                {
                    if(toFindCurr==toFindEnd)
                    {
                        if(pMatchEnd!=nullptr)
                            *pMatchEnd = matchCurr;
                        return matchBegin;
                    }
                    
                    if(matchCurr==end())
                    {
                        // remainder of string is shorter than string we search for.
                        // => there can not be any more matches.
                        if(pMatchEnd!=nullptr)
                            *pMatchEnd = end();
                        return end();
                    }
                    
                    if( (*matchCurr) != (*toFindCurr) )
                        break;
                    
                    toFindCurr++;
                    matchCurr++;
                }
            }
            
            if(pMatchEnd!=nullptr)
                *pMatchEnd = end();
            
            return end();
        }
        
        
        
        
        Iterator findNoCase(UniChar chr) const
        {
            return std::find(beginLower(), endLower(), charToLower(chr) );
        }
        
        Iterator findNoCase(UniChar chr, const Iterator& beginIt) const
        {
            return std::find( LoweringCharIterator(beginIt), endLower(), charToLower(chr) );
        }
        
        
        StringImpl<EncodedCharType, Alloc> replaceAll(UniChar oldChr, UniChar newChr) const
        {
            ReplacingCharIterator beginIt( begin(), oldChar, newChr );
            ReplacingCharIterator endIt( end(), oldChar, newChr );
            
            return StringImpl<EncodedCharType, Alloc>( beginIt, endIt);
        }
        
        template<class ToFindIterator>
        StringImpl<EncodedCharType, Alloc> replaceAll(const ToFindIterator& toFindBegin, const ToFindIterator& toFindEnd, const Iterator& replaceWithBegin, const Iterator& replaceWithEnd)
        {
            shared_ptr<EncodedStringType> pEncodedResult;
            
            Iterator posIt = begin();
            while( posIt!=end() )
            {
                Iterator matchEnd;
                Iterator matchBegin = find(toFindBegin, toFindEnd, posIt, &matchEnd);
                
                if(matchBegin==matchEnd)
                {
                    // no more matches.
                    
                    if(posIt==begin())
                    {
                        // nothing has been replaced. Just return a copy of us.
                        // Note that this is very cheap, since we will share the data object.
                        return *this;
                    }
                    else
                    {
                        pEncodedResult->append( posIt._encodedIt, end()._encodedIt );
                        break;
                    }
                }
                
                if(pEncodedResult==nullptr)
                    pEncodedResult = make_shared<EncodedStringType>();
                
                pEncodedResult->append( begin()._encodedIt, matchBegin._encodedIt );
                pEncodedResult->append( replaceWithBegin._encodedIt, replaceWithEnd._encodedIt );
                
                posIt = matchEnd;
            }
            
            return pEncodedResult;
        }
        
        
        template<class ToFindIterator, class InputIterator>
        StringImpl<EncodedCharType, Alloc> replaceAll(const ToFindIterator& toFindBegin, const ToFindIterator& toFindEnd, const InputIterator& replaceWithBegin, const Iterator& replaceWithEnd)
        {
            return replaceAll( toFindBegin, toFindEnd, EncodingIterator<EncoderType>(replaceWithBegin), EncodingIterator<EncoderType>(replaceWithEnd) );
        }
        
        
        StringImpl<EncodedCharType, Alloc> replaceAll(const StringImpl<EncodedCharType, Alloc>& toFind, const StringImpl<EncodedCharType, Alloc>& replaceWith)
        {
            return replaceAll(toFind.begin(), toFind.end(), replaceWith.begin(), replaceWith.end());
        }
        
        
        
        
        
        StringImpl<EncodedCharType, Alloc>& operator=(const StringImpl<EncodedCharType, Alloc>& o)
        {
            _pEncodedString = o._pEncodedString;
            return *this;
        }
        
        
        
        
    protected:
        shared_ptr<EncodedStringType> _pEncodedString;
    };
}


#endif

