#ifndef BDN_IteratorRangeCombiner_H_
#define BDN_IteratorRangeCombiner_H_

namespace bdn
{


/** Combines multiple iterator ranges into one.

Add multiple ranges with addRange(). Then iterators returns by begin() and end()
will iterate over the items of all the added ranges.
*/
template<class SubIteratorType>
class IteratorRangeCombiner : public Base
{
public:
	IteratorRangeCombiner()
	{
	}


	/** Adds a range to the combiner.

		@param beginIt iterator that points to the first element
		@param endIt iterator that points to the position just after the last element of the range to add.	
	*/
	void addRange( const SubIteratorType& beginIt, const SubIteratorType& endIt)
	{
		// do not add empty ranges! It makes everything more complicated.

		if(beginIt!=endIt)
			_rangeList.push_back( Range(beginIt, endIt) );
	}

	class Iterator : public std::iterator<std::bidirectional_iterator_tag, typename SubIteratorType::value_type >, public Base
	{
	protected:
		Iterator(IteratorRangeCombiner* pCombiner, bool setToEnd)
		{
			_pCombiner = pCombiner;

			if(pCombiner->_rangeList.empty() || setToEnd)
			{
				_rangeIt = _pCombiner->_rangeList.end();
				_atEnd = true;
			}
			else
			{
				_rangeIt = _pCombiner->_rangeList.begin();
				_subIt = _rangeIt->beginIt;
				_atEnd = false;
			}
		}

	public:
		
		Iterator& operator++()
		{
			++_subIt;
			if( _subIt == _rangeIt->endIt )
			{
				++_rangeIt;

				// we cannot initialize subIt here, since we might be at the end
				// of the range list.
				if(_rangeIt!= _pCombiner->_rangeList.end())
					_subIt = _rangeIt->beginIt;
				else
				{
					_atEnd = true;
					_subIt = SubIteratorType();
				}
			}

			return *this;
		}

		Iterator operator++(int)
		{
			Iterator oldVal = *this;
			operator++();

			return oldVal;
		}

		Iterator operator--()
		{
			if( _atEnd || _subIt == _rangeIt->beginIt )
			{
				_rangeIt--;

				// note that there are no empty ranges in our list.
				// So we know that the preceding range has at least one item.
				_subIt = _rangeIt->endIt;
			}

			_subIt--;

			_atEnd = false;

			return *this;
		}

		Iterator operator--(int)
		{
			Iterator oldVal = *this;
			operator--();

			return oldVal;
		}

		typename SubIteratorType::value_type operator*()
		{
			return *_subIt;
		}

		bool operator==(const Iterator& o) const
		{
			return (_rangeIt==o._rangeIt && (_atEnd || _subIt==o._subIt));
		}

		bool operator!=(const Iterator& o) const
		{
			return !operator==(o);
		}

	

		P<IteratorRangeCombiner>											_pCombiner;

		typename std::list<typename IteratorRangeCombiner::Range>::iterator	_rangeIt;
		SubIteratorType														_subIt;
		bool																_atEnd;

		friend class IteratorRangeCombiner;
	};	


	/** Returns an iterator to the beginning of the combined range.*/
	Iterator begin()
	{
		return Iterator(this, false);
	}

	/** Returns an iterator to the position just after the last element of the combined range.*/
	Iterator end()
	{
		return Iterator(this, true);
	}

protected:

	struct Range
	{
		Range(const SubIteratorType& beginIt, const SubIteratorType& endIt)
		{
			this->beginIt = beginIt;
			this->endIt = endIt;
		}

		SubIteratorType beginIt;
		SubIteratorType endIt;
	};

	std::list<Range>	_rangeList;
};

}

#endif
