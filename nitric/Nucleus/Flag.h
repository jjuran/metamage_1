// Flag.h

#ifndef NUCLEUS_FLAG_H
#define NUCLEUS_FLAG_H

#ifndef NUCLEUS_ENUMERATION_H
#include "Nucleus/Enumeration.h"
#endif


namespace Nucleus
{
	
	enum BitState
	{
		Off,
		On
	};
	
	template < class Tag, class Int >
	struct Flag
	{
		typedef Int                      IntegralType;
		typedef Enumeration< Tag, Int >  Enum;
		typedef typename Enum::Type      Type;
		
		enum Bit
		{
			kMinBit = 0,
			kMaxBit = Enum::bitWidth - 1
		};
		
		// The only defined bitshift operation is left-shift, for the purpose of
		// shifting a 1 bit to the left by some number of bits defined as a
		// constant for that purpose, and this mainly for defining the enum
		// constants.  Bitfields are not numbers as such and there's no
		// reason to shift the bits around as there is to set and clear them.
		
		friend Type operator<<( BitState value, Bit bit )
		{
			return Type( value << int( bit ) );
		}
		
		friend Type operator&( Type a, Type b )
		{
			return Type( IntegralType( a ) & IntegralType( b ) );
		}
		
		friend Type operator|( Type a, Type b )
		{
			return Type( IntegralType( a ) | IntegralType( b ) );
		}
		
		friend Type operator^( Type a, Type b )
		{
			return Type( IntegralType( a ) ^ IntegralType( b ) );
		}
		
		friend Type& operator&=( Type& a, Type b )
		{
			return a = a & b;
		}
		
		friend Type& operator|=( Type& a, Type b )
		{
			return a = a | b;
		}
		
		friend Type& operator^=( Type& a, Type b )
		{
			return a = a ^ b;
		}
	};
	
}

#define NUCLEUS_DEFINE_FLAG_OPS( EnumType )

#endif

