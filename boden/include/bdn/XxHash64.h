#ifndef BDN_XxHash64_H_
#define BDN_XxHash64_H_

#include <bdn/Number.h>

namespace bdn
{

/** Implementation of the xxHash64 hashing algorithm.

	A 32 bit version is also available - see \ref XxHash32

	xxHash is a very fast, high quality hashing algorithm for arbitrary
	binary data.

	*/
class XxHash64
{
public:
	
	/** Calculates the hash for the specified data.
	
		\c seed is an optional parameter that is usually left at its default
		value 0 for normal hashing. The seed can be used in special
		case to predictably alter the result. If you hash the same data with a different seed then
		you will get a different hash. 
	*/
	static uint64_t calcHash( const void* pData, int bytes, uint64_t seed = 0 )
	{
		SimpleDataProvider dataProvider( (const uint8_t*)pData, bytes );

		return calcHashWithDataProvider( dataProvider, seed);
	}

	struct TailData
	{
		/** Size of the tail data in bytes (must be <=31 ).*/
		size_t tailSizeBytes;

		/** Points to an external uint64_t array that contains the remaining
			64 bit values in the tail data. The array must have tailSizeBytes/8
			(rounded down) entries. So depending on the value of tailSizeBytes,
			this array can have between 0 and 3 entries.
			*/
		const uint64_t* p64BitValues;

		/** If the remaining tail data after the 64 bit values (see p64BitValues)
			is >= 4 bytes, then this points to an single uint32_t value, representing			
			the first 4 bytes of the remaining tail data, after the 64 bit values.
			The pointer is not used if the remaining tail data is less than 4 bytes.
			*/
		const uint32_t* p32BitValue;
		
		/** Points to an external uint8_t array that contains the 
			remaining bytes of the tail data, after the 64 bit values from p64BitValues
			and the 32 bit value from p32BitValue (if that is used). 
			The array must have (\c tailSizeBytes % 4) entries - so it is between 0 and 3 bytes.
			*/
		const uint8_t*  pBytes;
	};


	/** Calculates the hash, using data provided by the specified dataProvider object.
		This function can be used if not all the data is available at the same time,
		or if the data is not available in the form of a single buffer.

		xxHash internally regards the input data as a stream of blocks of four uint64_t values (32 bytes).
		After these blocks there can be an optional tail data block, consisting of 0-3 uint64_t values,
		0 or 1 32 bit value and 0-3 individual bytes. So the tail data can have a total size between 0 and 31 bytes.
				
		The data provider can be any object that provides the same functions as the
		following example class:

		\code

		class MyDataProvider
		{
		public:
			// Returns the next 32 byte block. The returned pointer must
			// point to an array of 4 uint64_t values. The memory of this array must be managed
			// by the data provider and must remain valid until the next data provider call.
			// Returns nullptr at the end, when no full 32 byte block is available anymore.
			const uint64_t* next4x8ByteBlock()
			{
				...
			}

			// Returns the remaining tail data after all available 32 byte blocks have been read
			// and less than 32 bytes of data remain. The hasher will call this exactly once, and
			// only after next4x8ByteBlock has returned nullptr.
			//
			// See TailData for information on the individual fields.
			//
			XxHash64::TailData getTailData()
			{
				...
			}
		};

		\endcode

		The \c seed parameter of calcHashWithDataProvider is an optional parameter that is usually left at its default
		value 0 for normal hashing. The seed can be used in special
		case to predictably alter the result. If you hash the same data with a different seed then
		you will get a different hash. 
	*/			
	template<typename DataProvider>
	static uint64_t calcHashWithDataProvider(DataProvider& dataProvider, uint64_t seed = 0 )
	{
		uint64_t	hash;

		size_t		totalByteCount = 0;

		const uint64_t* p4x8ByteBlock = dataProvider.next4x8ByteBlock();
		if( p4x8ByteBlock )
		{
			uint64_t	stateVal1 = seed + prime1 + prime2;
			uint64_t	stateVal2 = seed + prime2;
			uint64_t	stateVal3 = seed;
			uint64_t	stateVal4 = seed - prime1;

			do
			{
				totalByteCount += 32;

				doRound( stateVal1, *p4x8ByteBlock );
				p4x8ByteBlock++;

				doRound( stateVal2, *p4x8ByteBlock );
				p4x8ByteBlock++;

				doRound( stateVal3, *p4x8ByteBlock );
				p4x8ByteBlock++;

				doRound( stateVal4, *p4x8ByteBlock );
				
				p4x8ByteBlock = dataProvider.next4x8ByteBlock();
			}
			while( p4x8ByteBlock );

			hash = rotateBitsLeft( stateVal1, 1)
					+ rotateBitsLeft( stateVal2, 7)
					+ rotateBitsLeft( stateVal3, 12)
					+ rotateBitsLeft( stateVal4, 18);			

			doMergeRound(hash, stateVal1);
			doMergeRound(hash, stateVal2);
			doMergeRound(hash, stateVal3);
			doMergeRound(hash, stateVal4);
		}
		else
			hash = seed + prime5;

		TailData tailData = dataProvider.getTailData();

		totalByteCount += tailData.tailSizeBytes;

		hash += totalByteCount;

		
		const uint64_t* pTail64BitValuesEnd = tailData.p64BitValues + (tailData.tailSizeBytes >> 3);
		while(tailData.p64BitValues != pTail64BitValuesEnd)
		{
			uint64_t temp=0;

			doRound(temp, *tailData.p64BitValues );

			hash ^= temp;
			hash = rotateBitsLeft( hash, 27) * prime1 + prime4;
			
			tailData.p64BitValues++;
		}

		if( (tailData.tailSizeBytes & 7 ) >= 4 )
		{
			hash ^= ((uint64_t)(*tailData.p32BitValue)) * prime1;
			hash = rotateBitsLeft(hash, 23) * prime2 + prime3;			
		}

		const uint8_t* pTailBytesEnd = tailData.pBytes + (tailData.tailSizeBytes & 3);
		while( tailData.pBytes != pTailBytesEnd )
		{
			hash ^= (*tailData.pBytes) * prime5;
			hash = rotateBitsLeft(hash, 11) * prime1;
			
			tailData.pBytes++;
		}

		hash ^= hash >> 33;
		hash *= prime2;
		hash ^= hash >> 29;
		hash *= prime3;
		hash ^= hash >> 32;
		
		return hash;
	}
	

private:
	enum : uint64_t
	{
		prime1 = 11400714785074694791ULL,
		prime2 = 14029467366897019727ULL,
		prime3 = 1609587929392839161ULL,
		prime4 = 9650029242287828579ULL,
		prime5 = 2870177450012600261ULL
	};
		
	static void doRound(uint64_t& val, uint64_t inputValue)
	{
		val += inputValue * prime2;
		val = rotateBitsLeft(val, 31);
		val *= prime1;
	}

	static void doMergeRound(uint64_t& val, uint64_t inputValue)
	{
		uint64_t temp=0;
		doRound(temp, inputValue);

		val ^= temp;
		val = val * prime1 + prime4;
	}
	
	class SimpleDataProvider
	{
	public:
		SimpleDataProvider( const uint8_t* pData, size_t bytes )
			: _pNextData(pData)
			, _bytesLeft(bytes)
		{
		}

		const uint64_t* next4x8ByteBlock()
		{
			if( _bytesLeft < 32)
				return nullptr;
			
			memcpy( _8ByteValues, _pNextData, 32 );

#if BDN_IS_BIG_ENDIAN
			// in order for the hash to be the same on every system
			// we need to swap the byte order if we are on a big endian
			// machine.
			// This might not be necessary if a portable hash is not required.
			// But it is a very quick operation and this should be negligible
			// compared to the other operations we do. So we simply do this
			// always on big endian systems.
			swapByteOrder( _8ByteValues[0] );
			swapByteOrder( _8ByteValues[1] );
			swapByteOrder( _8ByteValues[2] );
			swapByteOrder( _8ByteValues[3] );
#endif

			_pNextData += 32;
			_bytesLeft -= 32;

			return _8ByteValues;
		}

		TailData getTailData()
		{
			memcpy( _8ByteValues, _pNextData, _bytesLeft );

			// note: casting the uint64_t pointer to uint32_t is fine, even
			// if memory access must be aligned. Because if alignment is necessary
			// then the 64bit value will be aligned on an 8 byte boundary.
			// And that means that it is also aligned on a 4 byte boundary.
			uint32_t* p4ByteValue = (uint32_t*) &_8ByteValues[ _bytesLeft/8 ];

			// note that p4ByteValue also points to the correct place
			// in the big endian case. Since we do not swap the byte
			// order of _8ByteValues[3], that means that it is still in little
			// endian order. So the "next" 32 bits will be at the beginning
			// of the 8 byte area.

#if BDN_IS_BIG_ENDIAN
			swapByteOrder( _8ByteValues[0] );
			swapByteOrder( _8ByteValues[1] );
			swapByteOrder( _8ByteValues[2] );
			
			swapByteOrder( *p4ByteValue );
#endif

			return TailData
				{
					_bytesLeft,
					_8ByteValues,
					p4ByteValue,
					_pNextData + _bytesLeft - (_bytesLeft & 3)
				};			
		}

	private:
		uint64_t		_8ByteValues[4];
		const uint8_t*	_pNextData;
		size_t			_bytesLeft;		
	};

};

}


#endif
