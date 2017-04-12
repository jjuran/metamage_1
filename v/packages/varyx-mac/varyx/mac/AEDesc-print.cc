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
		
		case 'furl':
		case 'utf8':
			count = ::AEGetDescDataSize( ptr );
			
			if ( count )
			{
				plus::string s;
				
				char* p = s.reset( count );
				
				::AEGetDescData( ptr, p, count );
				
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
