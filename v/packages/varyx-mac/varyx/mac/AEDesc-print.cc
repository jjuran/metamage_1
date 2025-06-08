/*
	AEDesc-print.cc
	---------------
*/

#include "varyx/mac/AEDesc-print.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#ifndef __AEDATAMODEL__
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// conv
#include "conv/utf8_16.hh"

// plus
#include "plus/mac_utf8.hh"

// vlib
#include "vlib/iterators/list_builder.hh"
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/types/float.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/mb32.hh"
#include "vlib/types/null.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/string.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"


namespace varyx
{
namespace mac
{

template < class Type, class Data >
static inline
void append_POD( list_builder& list, const ::AEDesc* desc )
{
	Data data;
	
	::AEGetDescData( desc, &data, sizeof data );
	
	list.append( Type( data ) );
}

static
OSType get_aevt_attr( const ::AEDesc* ptr, AEKeyword key )
{
	OSType code;
	
	OSErr err = ::AEGetAttributePtr( ptr, key, typeType, NULL, &code, 4, NULL );
	
	if ( err != noErr )
	{
		THROW( "AEGetAttributePtr() failed" );
	}
	
	return code;
}

static
plus::string UTF8_from_UTF16( const plus::string& utf16 )
{
	typedef plus::string::size_type size_t;
	
	size_t n_bytes = utf16.size();
	
	if ( n_bytes & 1 )
	{
		THROW( "invalid odd length for UTF-16 data from AEDesc" );
	}
	
	n_bytes /= 2;
	
	const UInt16* q = (const UInt16*) utf16.data();
	const UInt16* end = q + n_bytes;
	
	size_t length = conv::sizeof_utf8_from_utf16( q, end );
	
	plus::string buffer_utf8;
	
	char* p = buffer_utf8.reset( length );
	
	size_t result = conv::utf8_from_utf16_nothrow( p, length, q, n_bytes );
	
	if ( result == conv::invalid_utf16 )
	{
		THROW( "invalid UTF-16 data" );
	}
	
	return buffer_utf8;
}

plus::string printable_AEDesc( const ::AEDesc& desc )
{
	OSErr err;
	
	list_builder result;
	
	const ::AEDesc* ptr = &desc;
	
	const DescType descType = ptr->descriptorType;
	
	switch ( descType )
	{
		case 'aevt':
			result.append( MB32( get_aevt_attr( ptr, keyEventClassAttr ) ) );
			result.append( String( "/" ) );
			result.append( MB32( get_aevt_attr( ptr, keyEventIDAttr ) ) );
			break;
		
		default:
			result.append( MB32( descType ) );
	}
	
	bool is_aggregate = false;
	bool is_record    = false;
	
	switch ( descType )
	{
		case 'aevt':
		case 'obj ':
		case 'reco':
			is_record = true;
			// fall through
		
		case 'list':
			is_aggregate = true;
			break;
		
		default:
			break;
	}
	
	if ( ! is_aggregate )
	{
		result.append( String( "(" ) );
	}
	
	long        count;
	::AEKeyword keyword;
	::AEDesc    temp;
	
	switch ( descType )
	{
		case 'null':
			break;
		
		case 'enum':
		case 'type':
		case 'abso':
			append_POD< MB32, OSType >( result, ptr );
			break;
		
		case 'bool':  append_POD< Integer, UInt8  >( result, ptr );  break;
		case 'shor':  append_POD< Integer, SInt16 >( result, ptr );  break;
		case 'ushr':  append_POD< Integer, UInt16 >( result, ptr );  break;
		case 'long':  append_POD< Integer, SInt32 >( result, ptr );  break;
		case 'magn':  append_POD< Integer, UInt32 >( result, ptr );  break;
		
		case 'sing':  append_POD< Float, float  >( result, ptr );  break;
		case 'doub':  append_POD< Float, double >( result, ptr );  break;
		
		case 'TEXT':
		case 'furl':
		case 'utf8':
		case 'utxt':
			count = ::AEGetDescDataSize( ptr );
			
			if ( count )
			{
				plus::string s;
				
				char* p = s.reset( count );
				
				::AEGetDescData( ptr, p, count );
				
				if ( descType == 'TEXT' )
				{
					s = plus::utf8_from_mac( s );
				}
				else if ( descType == 'utxt' )
				{
					s = UTF8_from_UTF16( s );
				}
				
				s = rep( String( s ) );
				
				result.append( String( s ) );
			}
			break;
		
		case 'aevt':
		case 'obj ':
		case 'reco':
		case 'list':
			result.append( String( "{" ) );
			
			count = count_items( ptr );
			
			for ( int i = 1;  i <= count;  ++i )
			{
				err = ::AEGetNthDesc( ptr, i, typeWildCard, &keyword, &temp );
				
				if ( err != noErr )
				{
					THROW( "AEGetNthDesc() failed" );
				}
				
				plus::string printable;
				
				try
				{
					printable = printable_AEDesc( temp );
					
					::AEDisposeDesc( &temp );
				}
				catch ( ... )
				{
					::AEDisposeDesc( &temp );
					
					throw;
				}
				
				if ( keyword != '****' )
				{
					result.append( MB32( keyword ) );
					result.append( String( ": " ) );
				}
				
				result.append( String( printable ) );
				
				if ( i < count )
				{
					result.append( String( ", " ) );
				}
			}
			
			result.append( String( "}" ) );
			break;
		
		default:
			count = ::AEGetDescDataSize( ptr );
			
			if ( count )
			{
				result.append( String( "x\"" ) );
				
				plus::string s;
				
				char* p = s.reset( count );
				
				::AEGetDescData( ptr, p, count );
				
				result.append( Packed( s ) );
				result.append( String( "\"" ) );
			}
			break;
	}
	
	if ( ! is_aggregate )
	{
		result.append( String( ")" ) );
	}
	
	return str( result.get() );
}

}
}
