// Nucleus/Flag.h
// --------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


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
		
		/*
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
		*/
	};
	
	template < unsigned size > struct EnumSize_Traits;
	
	template <> struct EnumSize_Traits< sizeof (unsigned char     ) > { typedef unsigned char       Type; };
	template <> struct EnumSize_Traits< sizeof (unsigned short    ) > { typedef unsigned short      Type; };
	template <> struct EnumSize_Traits< sizeof (unsigned int      ) > { typedef unsigned int        Type; };
	template <> struct EnumSize_Traits< sizeof (unsigned long long) > { typedef unsigned long long  Type; };
	
}

#define NUCLEUS_DEFINE_FLAG_OPS( Enum )                               \
	inline Enum operator&( Enum a, Enum b )                           \
	{                                                                 \
		typedef Nucleus::EnumSize_Traits< sizeof (Enum) >::Type Int;  \
		return Enum( Int( a ) & Int( b ) );                           \
	}                                                                 \
	                                                                  \
	inline Enum operator|( Enum a, Enum b )                           \
	{                                                                 \
		typedef Nucleus::EnumSize_Traits< sizeof (Enum) >::Type Int;  \
		return Enum( Int( a ) | Int( b ) );                           \
	}                                                                 \
	                                                                  \
	inline Enum operator^( Enum a, Enum b )                           \
	{                                                                 \
		typedef Nucleus::EnumSize_Traits< sizeof (Enum) >::Type Int;  \
		return Enum( Int( a ) ^ Int( b ) );                           \
	}                                                                 \
	                                                                  \
	inline Enum& operator&=( Enum& a, Enum b )                        \
	{                                                                 \
		return a = a & b;                                             \
	}                                                                 \
	                                                                  \
	inline Enum& operator|=( Enum& a, Enum b )                        \
	{                                                                 \
		return a = a | b;                                             \
	}                                                                 \
	                                                                  \
	inline Enum& operator^=( Enum& a, Enum b )                        \
	{                                                                 \
		return a = a ^ b;                                             \
	}

#endif

