#ifndef BDN_hex_H_
#define BDN_hex_H_

namespace bdn
{


/** Returns true if the specified character is a hexadecimal digit (0-9, a-f, A-F).*/
inline bool isHexDigit(char32_t chr)
{
	return ( (chr>='0' && chr<='9')
			|| (chr>='a' && chr<='f')
			|| (chr>='A' && chr<='F') );
}


/** Decodes a hexadecimal digit and returns its value (0-15).
	Returns -1 if the character is not a hex digit.*/
inline int decodeHexDigit(char32_t chr)
{
	if(chr>='0' && chr<='9')
		return chr-'0';
	else if(chr>='a' && chr<='f')
		return chr-'a'+10;
	else if(chr>='A' && chr<='F')
		return chr-'A'+10;
	else
		return -1;
}


/** Encodes a number between 0 and 15 as a hexadecimal digit (0-f).
	Returns 0 if the number is not encodable in as a hexadecimal digit
	(i.e. if it is not between 0 and 15).*/
inline char32_t encodeHexDigit(int num)
{
	if(num<0)
		return 0;
	
	else if(num<=9)
		return '0'+num;

	else if(num<=15)
		return 'a'+num-10;

	else
		return 0;
}

}

#endif
