// Enumeration.h

#ifndef NUCLEUS_ENUMERATION_H
#define NUCLEUS_ENUMERATION_H

namespace Nucleus
{
	
	template < class Tag, class Int >
	struct Enumeration
	{
		typedef Int IntegralType;
		
		const static bool isSigned = IntegralType( -1 ) < IntegralType( 0 );
		
		const static int byteWidth = sizeof (IntegralType);
		const static int bitWidth  = byteWidth * 8;
		
		const static int binaryDigits = bitWidth - int( isSigned );
		
		const static IntegralType min = isSigned ? 1ULL << binaryDigits : 0;
		const static IntegralType max = IntegralType(~0) ^ (isSigned ? min : 0);
		
		enum Type
		{
			kMin = min,
			kMax = max
		};
	};
	
}

#endif

