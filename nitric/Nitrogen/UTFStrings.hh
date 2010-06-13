// Nitrogen/UTFStrings.hh
// ----------------------

// Written by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_UTFSTRINGS_HH
#define NITROGEN_UTFSTRINGS_HH

// Standard C++
#include <algorithm>
#include <string>

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#include "nucleus/convert.hh"
#include "nucleus/scribe.hh"

// Nitrogen
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif


namespace Nitrogen
{
	
	using ::UnicodeScalarValue;
	using ::UTF32Char;
	using ::UniChar;
	using ::UTF16Char;
	using ::UTF8Char;
	
	typedef std::basic_string< UTF32Char > UTF32String;
	typedef std::basic_string< UTF16Char > UTF16String;
	typedef std::basic_string< UTF8Char  > UTF8String;
	typedef std::basic_string< UniChar   > UniString;
	
	template < class UTFChar >
	struct UnicodeFlattener
	{
		typedef const std::basic_string< UTFChar >& Put_Parameter;
		
		template < class Putter >
		static void Put( Put_Parameter toPut, Putter put )
		{
			const UTFChar* begin = &toPut[0];
			const std::size_t size = toPut.size() * sizeof (UTFChar);
			
			put( begin, begin + size );
		}
		
		typedef std::basic_string< UTFChar > Get_Result;
		
		template < class Getter >
		static Get_Result Get( Getter get )
		{
			const std::size_t size = get.size();
			Get_Result result;
			
			result.resize( size / sizeof (UTFChar) );
			
			UTFChar* begin = &result[0];
			
			get( begin, begin + size );
			
			return result;
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
}

namespace nucleus
{
	
	template <> struct converter< Nitrogen::UniString, HFSUniStr255 >
	{
		typedef HFSUniStr255         argument_type;
		typedef Nitrogen::UniString  result_type;
		
		Nitrogen::UniString operator()( const HFSUniStr255& in ) const
		{
			return Nitrogen::UniString( in.unicode, in.unicode+in.length );
		}
	};
	
	template <> struct converter< HFSUniStr255, Nitrogen::UniString >
	{
		typedef Nitrogen::UniString  argument_type;
		typedef HFSUniStr255         result_type;
		
		HFSUniStr255 operator()( const Nitrogen::UniString& in ) const
		{
			if ( in.size() > 255 )
			{
				throw Nitrogen::StringTooLong();
			}
			
			HFSUniStr255 result;
			
			result.length = UInt16( in.size() );
			
			std::copy( in.begin(), in.end(), result.unicode );
			
			return result;
		}
	};
	
}

#endif
