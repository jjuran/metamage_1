// Nitrogen/Str.h
// --------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2008 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_STR_H
#define NITROGEN_STR_H

// Nucleus
#include "Nucleus/Convert.h"


namespace Nitrogen
{
	
	class StringTooLong {};
	
	void CopyToPascalString( const char *source,
	                         std::size_t sourceLength,
	                         unsigned char *destination,
	                         unsigned char destinationLength );
	
	inline void CopyToPascalString( const unsigned char *source,
	                                unsigned char *destination,
	                                unsigned char destinationLength )
	{
		CopyToPascalString( reinterpret_cast< const char* >( source + 1 ),
		                    source[ 0 ],
		                    destination,
		                    destinationLength );
	}
	
	inline void CopyToPascalString( const char *source,
	                                unsigned char *destination,
	                                unsigned char destinationLength )
	{
		CopyToPascalString( source,
		                    std::strlen( source ),
		                    destination,
		                    destinationLength );
	}
	
	inline  void CopyToPascalString( const std::string& source,
	                                 unsigned char *destination,
	                                 unsigned char destinationLength )
	{
		CopyToPascalString( source.data(),
		                    source.length(),
		                    destination,
		                    destinationLength );
	}
	                         
	template< unsigned char length >
	class Str
	{
		private:
			unsigned char string[ length + 1 ];
		
		public:
			Str()                                                    { string[ 0 ] = 0; }
			Str( const unsigned char* s )                            { CopyToPascalString( s, string, length ); }
			Str( const char* s )                                     { CopyToPascalString( s, string, length ); }
			Str( const char* s, std::size_t slen )                   { CopyToPascalString( s, slen, string, length ); }
			Str( const std::string& s )                              { CopyToPascalString( s, string, length ); }
			
			Str& operator=( const unsigned char* s )                 { CopyToPascalString( s, string, length ); return *this; } 
			Str& operator=( const char* s )                          { CopyToPascalString( s, string, length ); return *this; } 
			Str& operator=( const std::string& s )                   { CopyToPascalString( s, string, length ); return *this; } 
			
			operator unsigned char*()                                { return string; }
			operator const unsigned char *() const                   { return string; }
	};
	
	typedef Str< 255 > Str255;
	typedef Str<  63 >  Str63;
	typedef Str<  32 >  Str32;
	typedef Str<  31 >  Str31;
	typedef Str<  27 >  Str27;
	typedef Str<  15 >  Str15;
	
}

namespace Nucleus
{
	
	template <>
	struct Converter< std::string, const unsigned char * > : public std::unary_function< const unsigned char *, std::string >
	{
		std::string operator()( const unsigned char *input ) const;
	};
	
	template <>
	struct ConvertInputTraits< unsigned char* > : ConvertInputTraits< const unsigned char* > {};
	
	template < unsigned char length >
	struct ConvertInputTraits< const unsigned char[ length ] > : ConvertInputTraits< const unsigned char* > {};
	
#ifdef __APPLE_CC__
	
	// This must be present for gcc and absent for CodeWarrior.
	// If you're inclined to discover why this is, by all means be my guest.
	
	template < unsigned char length >
	struct ConvertInputTraits<       unsigned char[ length ] > : ConvertInputTraits< const unsigned char* > {};
	
#endif
	
	template < unsigned char length >
	struct ConvertInputTraits< Nitrogen::Str< length > > : ConvertInputTraits< const unsigned char* > {};
	
	// Convert StringHandle to std::string
	template <>
	struct Converter< std::string, unsigned char ** >: public std::unary_function< unsigned char **, std::string >
	{
		std::string operator()( unsigned char **input ) const
		{
			// We don't need to lock the handle because we copy it to the stack
			// before touching the heap, after which point we work from the copy.
			return Convert< std::string >( Nitrogen::Str255( *input ) );
		}
	};
	
}
	
#endif

