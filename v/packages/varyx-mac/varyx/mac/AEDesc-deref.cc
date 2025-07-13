/*
	AEDesc-deref.cc
	---------------
*/

#include "varyx/mac/AEDesc-deref.hh"

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
#include "vlib/array-utils.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/float.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/mb32.hh"
#include "vlib/types/null.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/string.hh"
#include "vlib/types/table.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"
#include "varyx/mac/FSRef.hh"
#include "varyx/mac/FSSpec.hh"
#include "varyx/mac/OSErr.hh"


namespace varyx
{
namespace mac
{

static
Value dereferenced_AEDesc( const ::AEDesc& desc )
{
	OSErr err;
	UInt8  u8;
	SInt16 i16;
	SInt32 i32;
	UInt32 u32;
	double f64;
	
	const ::AEDesc* ptr = &desc;
	
	const DescType descType = ptr->descriptorType;
	
	switch ( descType )
	{
		case 'null':
			return Null();
		
		case 'fals':
		case 'true':
			return vlib::Boolean( descType & 0x10000000 );
		
		case 'enum':
			::AEGetDescData( ptr, &u32, sizeof u32 );
			return MB32( u32 );
		
		case 'type':
		case 'abso':
			uint32_t type;
			::AEGetDescData( ptr, &type, 4 );
			return Value( MB32( descType ), Op_mapping, MB32( type ) );
		
		case 'bool':
			::AEGetDescData( ptr, &u8, sizeof u8 );
			return vlib::Boolean( u8 );
		
		case 'shor':
			::AEGetDescData( ptr, &i16, sizeof i16 );
			return Integer( i16 );
		
		case 'long':
			::AEGetDescData( ptr, &i32, sizeof i32 );
			return Integer( i32 );
		
		case 'magn':
			::AEGetDescData( ptr, &u32, sizeof u32 );
			return Integer( u32 );
		
		case 'doub':
			::AEGetDescData( ptr, &f64, sizeof f64 );
			return Float( f64 );
		
		/*
			Note:  We're not going to bring <Files.h> into
			scope just to get the size of FSSpec and FSRef.
		*/
		
	#if ! __LP64__
		
		case 'fss ':
		{
			enum { sizeof_FSSpec = 70 };
			
			FSSpec object;
			
			::FSSpec& pod = *object.pointer();
			
			::AEGetDescData( ptr, &pod, sizeof_FSSpec );
			
			return object;
		}
		
	#endif
		
		case 'fsrf':
		{
			enum { sizeof_FSRef = 80 };
			
			FSRef object;
			
			::FSRef& pod = *object.pointer();
			
			::AEGetDescData( ptr, &pod, sizeof_FSRef );
			
			return object;
		}
		
		case 'TEXT':
		case 'utxt':
		{
			AEDesc coerced;
			
			err = AECoerceDesc( ptr, 'utf8', coerced.pointer() );
			
			throw_MacOS_error( err, "AECoerceDesc" );
			
			return dereferenced_AEDesc( coerced.get() );
		}
		
		case 'obj ':
		case 'aevt':
		case 'reco':
		case 'list':
		{
			list_builder list;
			
			long count = count_items( ptr );
			
			for ( int i = 1;  i <= count;  ++i )
			{
				AEDesc element;
				::AEDesc* item = element.pointer();
				::AEKeyword keyword;
				
				err = ::AEGetNthDesc( ptr, i, typeWildCard, &keyword, item );
				
				throw_MacOS_error( err, "AEGetNthDesc" );
				
				Value result = dereferenced_AEDesc( *item );
				
				if ( descType != 'list' )
				{
					result = Value( MB32( keyword ), Op_mapping, result );
				}
				
				list.append( result );
			}
			
			Value result = make_array( list.get() );
			
			if ( descType != 'list' )
			{
				result = Table( Type( mb32_vtype ), result );
				
				if ( descType != 'reco' )
				{
					result = Value( MB32( descType ), Op_mapping, result );
				}
			}
			
			return result;
		}
		
		default:
			break;
	}
	
	long count = ::AEGetDescDataSize( ptr );
	
	plus::string s;
	char* p = s.reset( count );
	
	::AEGetDescData( ptr, p, count );
	
	switch ( descType )
	{
		case 'utf8':
			return String( s );
		
		case 'tdta':
			return Packed( s );
		
		default:
			break;
	}
	
	return Value( MB32( descType ), Op_mapping, Packed( s ) );
}

Value AEDesc_deref( const Value& v )
{
	const AEDesc& desc = static_cast< const AEDesc& >( v );
	
	return dereferenced_AEDesc( desc.get() );
}

}
}
