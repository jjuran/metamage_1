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
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif
#ifndef NUCLEUS_SCRIBE_HH
#include "nucleus/scribe.hh"
#endif

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_CFSTRING_HH
#include "Nitrogen/CFString.hh"
#endif
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
		typedef const std::basic_string< UTFChar >&  argument_type;
		typedef       std::basic_string< UTFChar >   result_type;
		
		template < class Putter >
		static void Put( argument_type toPut, Putter put )
		{
			const UTFChar* begin = &toPut[0];
			const std::size_t size = toPut.size() * sizeof (UTFChar);
			
			put( begin, begin + size );
		}
		
		template < class Getter >
		static result_type Get( Getter get )
		{
			const std::size_t size = get.size();
			
			result_type result;
			
			result.resize( size / sizeof (UTFChar) );
			
			UTFChar* begin = &result[0];
			
			get( begin, begin + size );
			
			return result;
		}
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
}

namespace Mac
{
	
	template <> struct DescType_scribe< typeUnicodeText > : type_< Nitrogen::UnicodeFlattener< UniChar > > {};
	
}

namespace nucleus
{
	
	template <>
	struct converter< Nitrogen::UniString, CFStringRef >
	{
		typedef CFStringRef          argument_type;
		typedef Nitrogen::UniString  result_type;
		
		Nitrogen::UniString operator()( const CFStringRef& in ) const
		{
			return Nitrogen::CFStringGetCharacters< Nitrogen::UniString >( in );
		}
	};
	
	template <>
	struct converter< nucleus::owned< CFStringRef >, Nitrogen::UniString >
	{
		typedef Nitrogen::UniString            argument_type;
		typedef nucleus::owned< CFStringRef >  result_type;
		
		nucleus::owned< CFStringRef > operator()( const Nitrogen::UniString& in ) const
		{
			return Nitrogen::CFStringCreateWithCharacters( in );
		}
	};
	
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
