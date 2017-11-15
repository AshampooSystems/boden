#ifndef BDN_XxHash32_H_
#define BDN_XxHash32_H_

#include <bdn/Number.h>

namespace bdn
{

/** Implementation of the xxHash32 hashing algorithm.

	A 64 bit version is also available - see \ref XxHash64

	xxHash is a very fast, high quality hashing algorithm for arbitrary
	binary data.

	*/
class XxHash32
{
public:
	
	/** Calculates the hash for the specified data.
	
		\c seed is an optional parameter that is usually left at its default
		value 0 for normal hashing. The seed can be used in special
		case to predictably alter the result. If you hash the same data with a different seed then
		you will get a different hash. 
	*/
	static uint32_t calcHash( const void* pData, int bytes, uint32_t seed = 0 )
	{
		SimpleDataProvider dataProvider( (const uint8_t*)pData, bytes );

		return calcHashWithDataProvider( dataProvider, seed);
	}

	struct TailData
	{
		/** Size of the tail data in bytes (must be <=15 ).*/
		size_t tailSizeBytes;

		/** Must point to an external uint32_t array that contains the remaining
			4 byte values in the tail data. The array must have tailSizeBytes/4
			(rounded down) entries. So depending on the value of tailSizeBytes,
			this array can have between 0 and 3 entries.
			*/
		const uint32_t* p32BitValues;
		
		/** Points to an external uint8_t array that contains the 
			remaining bytes of the tail data, after the 32 bit values from p32BitValues.
			The array must have (\c tailSizeBytes % 4) entries - so it is between 0 and 3 bytes.
			*/
		const uint8_t*  pBytes;
	};


	/** Calculates the hash, using data provided by the specified dataProvider object.
		This function can be used if not all the data is available at the same time,
		or if the data is not available in the form of a single buffer.

		xxHash internally regards the input data as a stream of blocks of four uint32_t values (16 bytes).
		After these blocks there can be an optional tail data block, consisting of 0-3 uint32_t values
		0-3 individual bytes. So the tail data can have a total size between 0 and 15 bytes.
				
		The data provider can be any object that provides the same functions as the
		following example class:

		\code

		class MyDataProvider
		{
		public:
			// Returns the next 16 byte block. The returned pointer must
			// point to an array of 4 uint32_t values. The memory of this array must be managed
			// by the data provider and must remain valid until the next data provider call.
			// Returns nullptr at the end, when no full 16 byte block is available anymore.
			const uint32* next4x4ByteBlock()
			{
				...
			}

			// Returns the remaining tail data after all available 16 byte blocks have been read
			// and less than 16 bytes of data remain. The hasher will call this exactly once, and
			// only after next4x4ByteBlock has returned nullptr.
			//
			// See TailData for information on the individual fields.
			//
			TailData getTailData()
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
	static uint32_t calcHashWithDataProvider(DataProvider& dataProvider, uint32_t seed = 0 )
	{
		uint32_t	hash;

		size_t		totalByteCount = 0;

		const uint32_t* p4x4ByteBlock = dataProvider.next4x4ByteBlock();
		if( p4x4ByteBlock )
		{
			uint32_t	stateVal1 = seed + prime1 + prime2;
			uint32_t	stateVal2 = seed + prime2;
			uint32_t	stateVal3 = seed;
			uint32_t	stateVal4 = seed - prime1;

			do
			{
				totalByteCount += 16;

				doRound( stateVal1, *p4x4ByteBlock );
				p4x4ByteBlock++;

				doRound( stateVal2, *p4x4ByteBlock );
				p4x4ByteBlock++;

				doRound( stateVal3, *p4x4ByteBlock );
				p4x4ByteBlock++;

				doRound( stateVal4, *p4x4ByteBlock );
				
				p4x4ByteBlock = dataProvider.next4x4ByteBlock();
			}
			while( p4x4ByteBlock );

			hash = rotateBitsLeft( stateVal1, 1)
					+ rotateBitsLeft( stateVal2, 7)
					+ rotateBitsLeft( stateVal3, 12)
					+ rotateBitsLeft( stateVal4, 18);			
		}
		else
			hash = seed + prime5;

		TailData tailData = dataProvider.getTailData();

		totalByteCount += tailData.tailSizeBytes;

		hash += totalByteCount;

		const uint32_t* pTail32BitValuesEnd = tailData.p32BitValues + (tailData.tailSizeBytes >> 2);
		while(tailData.p32BitValues != pTail32BitValuesEnd)
		{
			hash += (*tailData.p32BitValues) * prime3;
			hash = rotateBitsLeft( hash, 17 ) * prime4;

			tailData.p32BitValues++;
		}

		const uint8_t* pTailBytesEnd = tailData.pBytes + (tailData.tailSizeBytes & 3);
		while( tailData.pBytes != pTailBytesEnd )
		{
			hash += (*tailData.pBytes) * prime5;
			hash = rotateBitsLeft(hash, 11) * prime1;
			
			tailData.pBytes++;
		}

		hash ^= hash >> 15;
		hash *= prime2;
		hash ^= hash >> 13;
		hash *= prime3;
		hash ^= hash >> 16;
		
		return hash;
	}
	

private:
	enum : uint32_t
	{
		prime1 = 2654435761,
		prime2 = 2246822519,
		prime3 = 3266489917,
		prime4 = 668265263,
		prime5 = 374761393
	};

	static void doRound(uint32_t& val, uint32_t inputValue)
	{
		val += inputValue * prime2;
		val = rotateBitsLeft(val, 13);
		val *= prime1;
	}

	
	class SimpleDataProvider
	{
	public:
		SimpleDataProvider( const uint8_t* pData, size_t bytes )
			: _pNextData(pData)
			, _bytesLeft(bytes)
		{
		}

		const uint32_t* next4x4ByteBlock()
		{
			if( _bytesLeft < 16)
				return nullptr;
			
			memcpy( _4ByteValues, _pNextData, 16 );

#if BDN_IS_BIG_ENDIAN
			// in order for the hash to be the same on every system
			// we need to swap the byte order if we are on a big endian
			// machine.
			// This might not be necessary if a portable hash is not required.
			// But it is a very quick operation and this should be negligible
			// compared to the other operations we do. So we simply do this
			// always on big endian systems.
			swapByteOrder( _4ByteValues[0] );
			swapByteOrder( _4ByteValues[1] );
			swapByteOrder( _4ByteValues[2] );
			swapByteOrder( _4ByteValues[3] );
#endif

			_pNextData += 16;
			_bytesLeft -= 16;

			return _4ByteValues;
		}

		TailData getTailData()
		{
			memcpy( _4ByteValues, _pNextData, _bytesLeft );

#if BDN_IS_BIG_ENDIAN
			int fourByteValueCount = _bytesLeft / 4;
			for(int i=0; i<fourByteValueCount; i++)
				swapByteOrder( _4ByteValues[i] );
#endif

			return TailData
				{
					_bytesLeft,
					_4ByteValues,
					_pNextData + _bytesLeft - (_bytesLeft & 3)
				};			
		}

	private:
		uint32_t		_4ByteValues[4];
		const uint8_t*	_pNextData;
		size_t			_bytesLeft;		
	};

};

}


#endif
