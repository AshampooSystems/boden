#ifndef BDN_PreferredViewSizeManager_H_
#define BDN_PreferredViewSizeManager_H_

#include <set>

namespace bdn
{


/** Manages zur preferred size caching of views.

	The manager stores preferred sizes that a view has returned for a given amount of
	available space (see View::calcPreferredSize() ).

	It also implements some optimizations. It can sometimes infer the preferred size
	for an amount of available space, even when no size has been stored for that specific
	amount of space.

*/
class PreferredViewSizeManager : public Base
{
public:
	PreferredViewSizeManager()
	{
	}


	/** Stores a preferred size value for a given amount of available space.*/
	void set(const Size& availableSpace, const Size& preferredSize)
	{
		if(availableSpace == Size::none() )
		{
			_infiniteSpacePreferredSize = preferredSize;
			_haveInfiniteSpacePreferredSize = true;
		}
		else
			_entryMap[ Key{availableSpace} ] = preferredSize;
	}


	/** Gets the preferred size for the specified amount of available space.

		If a value is found in the cache then preferredSize is set to that value
		and true is returned.

		Otherwise preferredSize is set to Size::none() and false is returned.
		*/
	bool get(const Size& availableSpace, Size& preferredSize) const
	{
		// If we have a preferred size for infinite available space and
		// if the specified available space is bigger than the infinite space preferred size
		// then we can assume that it is correct and return the infinite one.
		if(_haveInfiniteSpacePreferredSize
			&& (!std::isfinite(availableSpace.width) || availableSpace.width >= _infiniteSpacePreferredSize.width )
			&& (!std::isfinite(availableSpace.height) || availableSpace.height >= _infiniteSpacePreferredSize.height ) )
		{
			preferredSize = _infiniteSpacePreferredSize;
			return true;
		}
		else
		{
			auto it = _entryMap.find( Key{availableSpace} );
			if( it == _entryMap.end() )
			{
				// we do not have an entry for this specific availableSpace.			
				preferredSize = Size::none();
				return false;
			}
			else
			{
				preferredSize = it->second;
				return true;
			}
		}
	}

private:

	struct Key
	{
		Size availableSpace;

		bool operator<(const Key& o) const
		{
			// we just need an arbitrary ordering on the available space.
			// Note that we cannot use Size::operator< because that will only return
			// true if both width and height are smaller than other size. So
			// the "not smaller and not bigger" set includes a lot of sizes for which
			// one component is smaller and the other bigger.

			// also note that any of these components might be infinity. But infinity compares
			// equal to itself so we do not have to check this in a special case.


			return ( availableSpace.width<0
					|| (availableSpace.width == o.availableSpace.width
						&& availableSpace.height < o.availableSpace.height) );
		}

	};

	std::map<Key, Size> _entryMap;
	bool			  _haveInfiniteSpacePreferredSize = false;
	Size			  _infiniteSpacePreferredSize;
};
    
}


#endif


