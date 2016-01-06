#ifndef _BDN_String_H_
#define _BDN_String_H_

#include <bdn/Base.h>

namespace bdn
{
    
    class String : public Base
    {
    public:
        String(const char* s, int lengthElements=-1)
        {
            _pData = make_shared<Utf8StringData>(s, lengthElements);
        }
        
        String(const char16_t* s, int lengthElements=-1)
        {
            _pData = make_shared<Utf16StringData>(s, lengthElements);
        }
        
        String(const char32_t* s, int lengthElements=-1)
        {
            _pData = make_shared<Utf32StringData>(s, lengthElements);
        }
        
        int getLength() const
        {
            return _pData->getLength();
        }
        
        int length() const
        {
            return _pData->getLength();
        }
        
        CharIterator begin()
        {
            return _pData->begin();
        }
        
        CharIterator end()
        {
            return _pData->end();
        }
        
       
        
        char32_t operator[](size_t index)
        {
            return pData->getChar(index);
        }
        
        bool operator==(const String& s)
        {
            return (*pData)==(*s._pData);
        }
        
        
        CharIterator find(const String& subString) const
        {
            return _pData->find(subString.begin(), subString.end());
        }
        
        CharIterator find(CharIterator beginIt, CharIterator endIt) const
        {
            return _pData->find(beginIt, endIt);
        }
        
        
        
        CharIterator findNoCase(const String& subString) const
        {
            return _pData->findNoCase(subString.begin(), subString.end());
        }
        
        CharIterator findNoCase(CharIterator beginIt, CharIterator endIt) const
        {
            return _pData->findNoCase(beginIt, endIt);
        }

        
        
        String replace(const String& oldSubString, const String& newSubString) const
        {
            return _pData->replace(oldSubString.begin(), oldSubString.end(), newSubString.begin(), newSubString.end());
        }
        
        String replace(CharIterator oldBegin, CharIterator oldEnd, CharIterator newBegin, CHarIterator newEnd) const
        {
            return _pData->replace(oldBegin, oldEnd, newBegin, newEnd);
        }

        
        
        shared_ptr<const StringDataBase> getData() const
        {
            return _pData;
        }
        
        template class<DATATYPE>
        shared_ptr<DATATYPE> getDataAs() const
        {
            shared_ptr<DATATYPE> pResult = dynamic_pointer_cast<DATATYPE>( _pData );
            if(pResult==nullptr)
            {
                // convert to the desired type
                pResult = make_shared<DATATYPE>(_pData);
                
                // replace the "old" representation. We assume that we will
                // more often need the requested type than our previous internal type.
                _pData = pResult;
            }
            
            return pResult;
        }
        
        
        shared_ptr<const Utf8StringData> getUtf8() const
        {
            return getDataAs<Utf8StringData>();
        }
        
        shared_ptr<const Utf16StringData> getUtf16() const
        {
            return getDataAs<Utf16StringData>();
        }
        
        shared_ptr<const Utf32StringData> getUtf32() const
        {
            return getDataAs<Utf32StringData>();
        }
        
        
        const char* getUtf8Ptr() const
        {
            return getUtf8()->getRawData();
        }
        
        const char16_t* getUtf16Ptr() const
        {
            return getUtf16()->getRawData();
        }
        
        const char32_t getUtf32Ptr() const
        {
            return getUtf32()->getRawData();
        }
        
        
        

    protected:
        mutable shared_ptr< StringDataBase > _pData;
    };
}


#endif

