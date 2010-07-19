// Carbonate/CFString.cc

// Mac OS
#ifndef __CFSTRING__
#include <CFString.h>
#endif

// Standard C++
#include <algorithm>

// Standard C
#include <string.h>

// chars
#include "charsets/MacRoman.hh"
#include "encoding/utf8.hh"

// Carbonate
#include "Carbonate/CFString_UTF8.hh"

#if !TARGET_API_MAC_CARBON

#include "Carbonate/CFBase.hh"


typedef chars::unichar_t (*Unicode_getter)( const char*& p, const char* end );

typedef unsigned (*Unicode_putter)( chars::unichar_t uc, char* p, const char* end );

class invalid_UTF8_data {};

const CFStringEncoding kCFString_constant = kCFStringEncodingASCII | 0x80000000;


struct __CFString : CFObject
{
	CFStringEncoding encoding;
	
	UInt32 n_bytes;
	UInt32 n_chars;
	
	union
	{
		const char*  codes;
		void*        raw_data;
	};
	
	__CFString() : encoding(), n_bytes(), n_chars(), codes()
	{
	}
	
	__CFString( const char* p, UInt32 n );
	
	__CFString( const char* p, UInt32 n, CFStringEncoding e );
	
	__CFString( const UTF16Char* u, UInt32 n_chars );
	
	~__CFString();
};

__CFString::__CFString( const char* p, UInt32 n )
:
	encoding( kCFString_constant ),
	n_bytes( n ),
	n_chars( n ),
	codes( p )
{
}

__CFString::__CFString( const char* p, UInt32 n, CFStringEncoding e )
:
	encoding( e ),
	n_bytes( n ),
	n_chars( e == kCFStringEncodingUTF8 ? count_UTF8_chars( p, p + n ) : n )
{
	if ( n > 0  &&  n_chars == 0 )
	{
		throw invalid_UTF8_data();
	}
	
	raw_data = ::operator new( n_bytes );
	
	memcpy( raw_data, p, n_bytes );
}

__CFString::~__CFString()
{
	if ( encoding != kCFString_constant )
	{
		::operator delete( raw_data );
	}
}

CFStringRef CFStringCreateWithPascalString( CFAllocatorRef    alloc,
                                            ConstStr255Param  pStr,
                                            CFStringEncoding  encoding )
{
	return CFStringCreateWithBytes( alloc,
	                                pStr + 1,
	                                pStr[ 0 ],
	                                encoding,
	                                false );
}

CFStringRef CFStringCreateWithCString( CFAllocatorRef    alloc,
                                       const char*       cStr,
                                       CFStringEncoding  encoding )
{
	return CFStringCreateWithBytes( alloc,
	                                (const UInt8*) cStr,
	                                strlen( cStr ),
	                                encoding,
	                                false );
}

CFStringRef CFStringCreateWithCharacters( CFAllocatorRef  alloc,
                                          const UniChar*  chars,
                                          CFIndex         numChars )
{
	return new __CFString( chars, numChars );
}

CFIndex CFStringGetLength( CFStringRef string )
{
	return string ? string->n_chars : 0;
}


CFIndex CFStringGetBytes( CFStringRef       string,
                          CFRange           range,
                          CFStringEncoding  encoding,
                          UInt8             lossByte,
                          Boolean           isExternalRepresentation,
                          UInt8 *           buffer,
                          CFIndex           maxBufLen,
                          CFIndex *         usedBufLen )
{
	*usedBufLen = 0;
	
	if ( string == NULL  ||  range.location < 0  ||  lossByte  ||  isExternalRepresentation )
	{
		return 0;
	}
	
	const CFIndex n_chars = std::min< CFIndex >( string->n_chars - range.location, range.length );
	
	if ( n_chars <= 0 )
	{
		return 0;
	}
	
	const char* p = string->codes;
	
	const char* codes_end = p + string->n_bytes;
	
	Unicode_getter getter = NULL;
	
	switch ( string->encoding )
	{
		case kCFStringEncodingMacRoman:
			getter = &chars::get_next_code_point_from_extended_ascii< chars::MacRoman_decoder_table >;
			break;
		
		case kCFStringEncodingUTF8:
			getter = &chars::get_next_code_point_from_utf8;
			break;
		
		default:
			return 0;
	}
	
	if ( (string->encoding & 0xFFFF ) == 0x0100 )
	{
		for ( int i = range.location;  i != 0;  --i )
		{
			(void) getter( p, codes_end );
		}
	}
	else
	{
		p += range.location;
	}
	
	Unicode_putter putter = NULL;
	
	switch ( encoding )
	{
		case kCFStringEncodingMacRoman:
			putter = &chars::put_code_point_into_extended_ascii< chars::MacRoman_encoder_map >;
			break;
		
		case kCFStringEncodingUTF8:
			putter = &put_code_point_into_UTF8;
			break;
		
		default:
			return 0;
	}
	
	char scratch_buffer[ 8 ];
	
	char* q = buffer ? (char*) buffer : scratch_buffer;
	
	const char* buffer_end = q + (buffer ? maxBufLen : sizeof scratch_buffer);
	
	CFIndex result = 0;
	
	while ( result < n_chars )
	{
		chars::unichar_t uc = getter( p, codes_end );
		
		if ( uc == chars::unichar_t( -1 ) )
		{
			break;
		}
		
		const unsigned size = putter( uc, q, buffer_end );
		
		if ( size == 0 )
		{
			break;
		}
		
		if ( buffer != NULL )
		{
			q += size;
		}
		
		*usedBufLen += size;
		
		++result;
	}
	
	return result;
}

CFStringRef CFStringCreateWithBytes( CFAllocatorRef    alloc,
                                     const UInt8 *     bytes,
                                     CFIndex           numBytes,
                                     CFStringEncoding  encoding,
                                     Boolean           isExternalRepresentation )
{
	if ( numBytes < 0 )
	{
		return NULL;
	}
	
	if ( isExternalRepresentation )
	{
		return NULL;  // not supported
	}
	
	try
	{
		return new __CFString( (const char*) bytes, numBytes, encoding );
	}
	catch ( ... )
	{
		return NULL;
	}
}

#endif

