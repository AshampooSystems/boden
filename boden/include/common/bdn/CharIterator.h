#ifndef BDN_CharIterator_H_
#define BDN_CharIterator_H_


#include <iterator>

namespace bdn
{
    

	/** Base class for character iterators.*/
    class CharIterator : public std::iterator<UniChar, std::bidirectional_iterator_tag>
    {
    public:
        
		virtual UniChar operator*() const = 0;
        
		virtual CharIterator operator++() = 0;
        virtual CharIterator operator--() = 0;
        
		virtual bool operator==(const CharIterator& o) const = 0;
        
        virtual bool operator!=(const CharIterator& o) const
        {
			return !operator==(o);
        }
        
    };
    
    
}


#endif