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


#if UNIVERSAL_INTERFACES_VERSION < 0x0400
enum { typeData = 'tdta' };
#endif

namespace varyx
{
namespace mac
{

DescType undefined_floating_point_type();

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
				i64 = v.number().clipped();
				
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
