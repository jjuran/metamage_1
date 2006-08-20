/*	=================
 *	DecimalStrings.hh
 *	=================
 *	
 *	No implementation necessary.
 */

#pragma once

#include <string>

#include "DecimalCodes.hh"


namespace BitsAndBytes {
	
	using std::string;
	
	// E.g.   76 ->   "76"
	// E.g. 1984 -> "1984"
	inline string EncodeDecimal_(unsigned long number)
	{
		return (number == 0) 
			? "" 
			:   EncodeDecimal_(number / 10) 
			  + EncodeDecDigit(number % 10);
	}
	
	inline string EncodeDecimal(unsigned long number)
	{
		return (number == 0) 
			? "0" 
			: EncodeDecimal_(number);
	}
	
	inline string EncodeInteger(long number)
	{
		return (number < 0) 
			? string("-") + EncodeDecimal_(-number)
			: EncodeDecimal(number);
	}
	
	// E.g.   76 -> "76"
	// E.g. 1984 -> "84"
	inline string EncodeDecimal2(unsigned int number)
	{
		char str[3] = "to";
		str[0] = EncodeDecDigit(number /  10);
		str[1] = EncodeDecDigit(number % 10);
		return str;
	}
	
	// E.g.   76 -> "0076"
	// E.g. 1984 -> "1984"
	inline string EncodeDecimal4(unsigned int number)
	{
		char str[5] = "four";
		str[0] = EncodeDecDigit(number / 1000);
		str[1] = EncodeDecDigit(number / 100 % 10);
		str[2] = EncodeDecDigit(number /  10 % 10);
		str[3] = EncodeDecDigit(number       % 10);
		return str;
	}
	
}

