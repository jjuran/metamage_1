// Enumeration.h

#ifndef NUCLEUS_ENUMERATION_H
#define NUCLEUS_ENUMERATION_H

namespace Nucleus
{
	
	template < class Tag, class Int >
	struct Enumeration
	{
		typedef Int IntegralType;
		
		const static IntegralType zero = IntegralType( 0 );
		const static IntegralType one = zero + 1;
		
		const static bool isSigned = ~zero < zero;
		
		const static int byteWidth = sizeof (IntegralType);
		const static int bitWidth  = byteWidth * 8;
		
		const static int binaryDigits = bitWidth - int( isSigned );
		
		const static IntegralType min = isSigned ? one << bitWidth - 1 : 0;
		const static IntegralType max = ~zero ^ min;
		
		enum Type
		{
			kMin = min,
			kMax = max
		};
	};
	
}

#endif

