/*
	AEDesc-coerce.cc
	----------------
*/

#include "varyx/mac/AEDesc-coerce.hh"

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
#ifndef __FILES__
#include <Files.h>
#endif

// chars
#include "charsets/ascii.hh"
#include "conv/mac_utf8.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/float.hh"
#include "vlib/types/fraction.hh"
#include "vlib/types/invocation.hh"
#include "vlib/types/mb32.hh"
#include "vlib/types/null.hh"
#include "vlib/types/record.hh"
#include "vlib/types/string.hh"
#include "vlib/types/table.hh"

// varyx-mac
#include "varyx/mac/FSSpec.hh"


#if UNIVERSAL_INTERFACES_VERSION < 0x0400
enum { typeData = 'tdta' };
#endif

namespace varyx
{
namespace mac
{

DescType undefined_floating_point_type();

static
AEDesc coerce_string_to_AEDesc( const plus::string& s )
{
	using std::size_t;
	
	plus::string result;
	
	::DescType     type = 0;
	const void*    data = NULL;
	unsigned long  size = 0;
	
	size_t input_size = s.size();
	const char* begin = s.data();
	const char* end   = begin + input_size;
	
	size = conv::sizeof_mac_from_utf8( begin, end );
	
	char* p = result.reset( size );
	
	size = conv::mac_from_utf8_nothrow( p, size, begin, input_size );
	
	if ( (long) size >= 0 )
	{
		// Successful conversion of UTF-8 to MacRoman
		
		type = typeChar;
		data = result.data();
	//	size = ...;
	}
	else
	{
		// Either invalid UTF-8, or non-MacRoman UTF-8 code points
		
		enum
		{
			typeData     = 'tdta',
			typeUTF8Text = 'utf8',
		};
		
		type = size == conv::non_Mac_utf8 ? typeUTF8Text : typeData;
		data = begin;
		size = input_size;
	}
	
	return AEDesc( type, data, size );
}

static
AEDesc coerce_mappings_to_AEDesc( const Value& mappings )
{
	AEDesc desc( AEDesc::Create_AERecord );
	
	::AEDesc* reco = desc.pointer();
	
	list_iterator it( mappings );
	
	while ( it )
	{
		Expr* expr = it.use().expr();
		
		const Value& key   = expr->left;
		const Value& value = expr->right;
		
		const ::AEKeyword keyword = key.to< MB32 >().get();
		
		const AEDesc item = coerce_to_AEDesc( value );
		
		OSErr err = ::AEPutKeyDesc( reco, keyword, &item.get() );
		
		throw_MacOS_error( err, "AEPutKeyDesc" );
	}
	
	return desc;
}

AEDesc coerce_to_AEDesc( const Value& v )
{
	if ( v.dispatch_methods() == &AEDesc_dispatch )
	{
		return static_cast< const AEDesc& >( v );
	}
	
	OSErr err;
	
	if ( is_array( v ) )
	{
		AEDesc desc( AEDesc::Create_AEDescList );
		
		if ( v.expr() )
		{
			::AEDesc* list = desc.pointer();
			
			list_iterator it( v.expr()->right );
			
			while ( it )
			{
				const AEDesc item = coerce_to_AEDesc( it.use() );
				
				err = ::AEPutDesc( list, 0, &item.get() );
				
				throw_MacOS_error( err, "AEPutDesc" );
			}
		}
		
		return desc;
	}
	
	if ( v.is< Table >() )
	{
		const Value& array = v.expr()->right;
		
		return coerce_mappings_to_AEDesc( array.expr() ? array.expr()->right
		                                               : empty_list );
	}
	
	if ( Expr* expr = v.expr() )
	{
		if ( expr->op == Op_record )
		{
			return coerce_mappings_to_AEDesc( expr->right );
		}
		
		if ( expr->op == Op_invocation )
		{
			if ( const Value& mappings = dereference_block( v ) )
			{
				return coerce_mappings_to_AEDesc( mappings );
			}
		}
		
		if ( expr->op == Op_mapping )
		{
			const Value& key = expr->left;
			const Value& value = expr->right;
			
			const MB32 descType = key.to< MB32 >();
			
			AEDesc desc = coerce_to_AEDesc( value );
			
			desc.pointer()->descriptorType = descType.get();
			
			return desc;
		}
	}
	
	::DescType     type = 0;
	const void*    data = NULL;
	unsigned long  size = 0;
	
	switch ( v.type() )
	{
		case Value_empty_list:
			type = typeNull;
			break;
		
		case V_pack:
			type = typeData;
			data = v.string().data();
			size = v.string().size();
			break;
		
		case V_str:
			type = typeChar;
			data = v.string().data();
			size = v.string().size();
			
			const char* begin;
			begin = (const char*) data;
			
			if ( chars::find_non_ascii( begin, begin + size ) )
			{
				return coerce_string_to_AEDesc( v.string() );
			}
			
			break;
		
		case Value_mb32:
			type = typeType;
			data = &v.to< MB32 >().get();
			size = sizeof (OSType);
			break;
		
		case V_bool:
			type = v.to< vlib::Boolean >() ? typeTrue : typeFalse;
			break;
		
		case V_int:
			SInt16 i16;
			SInt32 i32;
			UInt32 u32;
			SInt64 i64;
			if ( v.number().demotes_to< SInt16 >() )
			{
				i16 = v.number().clipped();
				
				type = typeSInt16;
				data = &i16;
				size = sizeof i16;
			}
			else if ( v.number().demotes_to< SInt32 >() )
			{
				i32 = v.number().clipped();
				
				type = typeSInt32;
				data = &i32;
				size = sizeof i32;
			}
			else if ( v.number().demotes_to< UInt32 >() )
			{
				u32 = v.number().clipped();
				
				type = typeUInt32;
				data = &u32;
				size = sizeof u32;
			}
			else if ( v.number().demotes_to< SInt64 >() )
			{
				i64 = v.number().clipped_to< SInt64 >();
				
				type = typeSInt64;
				data = &i64;
				size = sizeof i64;
			}
			break;
		
		default:
			break;
	}
	
	if ( const Float* f = v.is< Float >() )
	{
		type = sizeof (float_type) ==  4 ? typeIEEE32BitFloatingPoint
		     : sizeof (float_type) ==  8 ? typeIEEE64BitFloatingPoint
		     : sizeof (float_type) == 16 ? type128BitFloatingPoint
		     :                             undefined_floating_point_type();
		
		data = &f->get();
		size = sizeof (float_type);
	}
	
	if ( v.is< Fraction >() )
	{
		return coerce_to_AEDesc( v.to< Float >() );
	}
	
	if ( v.is< Null >() )
	{
		type = typeNull;
	}
	
#if ! __LP64__
	
	if ( const FSSpec* fsspec = v.is< FSSpec >() )
	{
		const ::FSSpec& spec = fsspec->get();
		
		type = typeFSS;
		data = &spec;
		size = sizeof spec;
	}
	
#endif
	
	if ( type == 0 )
	{
		Value desc  = mapping( "desc", String( "No conversion to AEDesc" ) );
		Value value = mapping( "value", v );
		
		Value mappings( desc, value );
		
		const Value exception = Record( mappings );
		
		throw user_exception( exception, source_spec() );
	}
	
	return AEDesc( type, data, size );
}

}
}
