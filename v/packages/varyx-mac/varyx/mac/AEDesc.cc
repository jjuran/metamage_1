/*
	AEDesc.cc
	---------
*/

#include "varyx/mac/AEDesc.hh"

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

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/targets.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/mb32.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/record.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc-coerce.hh"
#include "varyx/mac/AEDesc-deref.hh"


namespace varyx
{
namespace mac
{

void throw_MacOS_error( int err, const char* routine )
{
	if ( err  )
	{
		Value code = mapping( "OSErr", Integer( err ) );
		Value from = mapping( "from", String( routine ) );
		
		const Value exception = Record( Value( code, from ) );
		
		throw user_exception( exception, source_spec() );
	}
}

Value AEDesc::coerce( const Value& v )
{
	return coerce_to_AEDesc( v );
}

static
const char* AEDesc_str_data( const Value& v )
{
	return "<AEDesc>";
}

static const stringify AEDesc_str =
{
	&AEDesc_str_data,
	NULL,
	NULL,
};

static
size_t AEDesc_bin_size( const Value& v )
{
	const AEDesc& desc = static_cast< const AEDesc& >( v );
	
	return desc.data_size();
}

static
char* AEDesc_bin_copy( char* p, const Value& v )
{
	const AEDesc& desc = static_cast< const AEDesc& >( v );
	
	return desc.copy_data( p );
}

static const stringify AEDesc_bin =
{
	NULL,
	&AEDesc_bin_size,
	&AEDesc_bin_copy,
};

const stringifiers AEDesc_stringifiers =
{
	&AEDesc_str,
	NULL,         // rep: ditto
	&AEDesc_bin,
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	switch ( op )
	{
		case Op_typeof:
			return Type( AEDesc_vtype );
		
		case Op_unary_deref:
			return AEDesc_deref( v );
		
		default:
			break;
	}
	
	return Value();
}

static
Value AEDescList_subscript( const ::AEDesc& list, const Integer& index )
{
	OSErr err;
	
	::AEKeyword keyword;
	
	int32_t i = integer_cast< int32_t >( index );
	
	AEDesc nth;
	
	err = ::AEGetNthDesc( &list, i, typeWildCard, &keyword, nth.pointer() );
	
	throw_MacOS_error( err, "AEGetNthDesc" );
	
	if ( keyword == typeWildCard )
	{
		return nth;
	}
	
	return Value( MB32( keyword ), Op_mapping, nth );
}

static
Value AERecord_subscript( const ::AEDesc& record, const MB32& key )
{
	OSErr err;
	
	AEDesc keyed;
	
	err = ::AEGetKeyDesc( &record, key.get(), typeWildCard, keyed.pointer() );
	
	throw_MacOS_error( err, "AEGetKeyDesc" );
	
	return keyed;
}

static
Value binary_op_handler( op_type op, const Value& a, const Value& b )
{
	const ::AEDesc& desc = static_cast< const AEDesc& >( a ).get();
	
	switch ( op )
	{
		case Op_subscript:
			if ( const Integer* index = b.is< Integer >() )
			{
				return AEDescList_subscript( desc, *index );
			}
			else if ( const MB32* key = b.is< MB32 >() )
			{
				return AERecord_subscript( desc, *key );
			}
			else if ( const String* s = b.is< String >() )
			{
				return AERecord_subscript( desc, b.to< MB32 >() );
			}
			break;
		
		default:
			break;
	}
	
	return NIL;
}

static
Value append_to_list( Value& list, const Value& item )
{
	OSErr err;
	
	::AEDesc* listDesc = (::AEDesc*) &static_cast< AEDesc& >( list ).get();
	
	if ( Expr* expr = item.expr() )
	{
		if ( expr->op == Op_mapping )
		{
			const Value& key = expr->left;
			const Value& value = expr->right;
			
			const MB32 keyword = key.to< MB32 >();
			
			AEDesc desc = coerce_to_AEDesc( value );
			
			err = ::AEPutKeyDesc( listDesc, keyword.get(), &desc.get() );
			
			throw_MacOS_error( err, "AEPutKeyDesc" );
			
			return list;
		}
	}
	
	AEDesc desc = coerce_to_AEDesc( item );
	
	err = ::AEPutDesc( listDesc, 0, &desc.get() );
	
	throw_MacOS_error( err, "AEPutDesc" );
	
	return list;
}

static
Value mutating_op_handler( op_type        op,
                           const Target&  target,
                           const Value&   x,
                           const Value&   b )
{
	switch ( op )
	{
		case Op_push:
			return append_to_list( *target.addr, b );
		
		default:
			break;
	}
	
	return Value();
}

static const operators ops =
{
	&unary_op_handler,
	&binary_op_handler,
	NULL,
	&mutating_op_handler,
};

const dispatch AEDesc_dispatch =
{
	&AEDesc_stringifiers,
	NULL,
	NULL,
	&ops,
};

static
void AEDesc_destructor( void* ptr )
{
	::AEDesc* desc = (::AEDesc*) ptr;
	
	::AEDisposeDesc( desc );
}

AEDesc::AEDesc()
:
	Value( sizeof (::AEDesc),
	       &AEDesc_destructor,
	       Value_other,
	       &AEDesc_dispatch )
{
	::AEDesc& desc = *(::AEDesc*) pointer();
	
	desc.descriptorType = typeNull;
	desc.dataHandle     = NULL;
}

AEDesc::AEDesc( unsigned long type, const void* data, unsigned long size )
:
	Value( sizeof (::AEDesc),
	       &AEDesc_destructor,
	       Value_other,
	       &AEDesc_dispatch )
{
	::AEDesc& desc = *(::AEDesc*) pointer();
	
	desc.descriptorType = typeNull;
	desc.dataHandle     = NULL;
	
	OSErr err;
	
	err = ::AECreateDesc( type, data, size, &desc );
	
	throw_MacOS_error( err, "AECreateDesc" );
}

AEDesc::AEDesc( list_type_to_create list_type )
:
	Value( sizeof (::AEDesc),
	       &AEDesc_destructor,
	       Value_other,
	       &AEDesc_dispatch )
{
	::AEDesc& desc = *(::AEDesc*) pointer();
	
	desc.descriptorType = typeNull;
	desc.dataHandle     = NULL;
	
	OSErr err;
	
	err = ::AECreateList( 0, 0, list_type, &desc );
	
	throw_MacOS_error( err, "AECreateList" );
}

Value AEDesc::get_descriptorType() const
{
	const ::AEDesc* desc = (const ::AEDesc*) pointer();
	
	return MB32( desc->descriptorType );
}

Value AEDesc::get_dataHandle() const
{
	const ::AEDesc* desc = (const ::AEDesc*) pointer();
	
	OSErr err;
	
	Size size = ::AEGetDescDataSize( desc );
	
	plus::string data;
	
	char* p = data.reset( size );
	
	err = ::AEGetDescData( desc, p, size );
	
	throw_MacOS_error( err, "AEGetDescData" );
	
	return Packed( data );
}

unsigned long AEDesc::data_size() const
{
	const ::AEDesc* desc = (const ::AEDesc*) pointer();
	
	return ::AEGetDescDataSize( desc );
}

char* AEDesc::copy_data( char* p ) const
{
	const ::AEDesc* desc = (const ::AEDesc*) pointer();
	
	Size size = ::AEGetDescDataSize( desc );
	
	OSErr err = ::AEGetDescData( desc, p, size );
	
	throw_MacOS_error( err, "AEGetDescData" );
	
	return p + size;
}

static
Value v_to( const Value& args )
{
	const AEDesc& desc = static_cast< const AEDesc& >( first( args ) );
	const MB32&   type = static_cast< const MB32&   >( rest ( args ) );
	
	AEDesc coerced;
	
	OSErr err = AECoerceDesc( &desc.get(), type.get(), coerced.pointer() );
	
	throw_MacOS_error( err, "AECoerceDesc" );
	
	return coerced;
}

static const Type mb32 = mb32_vtype;

static const Value AEDesc_mb32( (Type( AEDesc_vtype )), mb32 );

static const proc_info proc_to = { "to", &v_to, &AEDesc_mb32 };

static
Value AEDesc_member( const Value&         obj,
                     const plus::string&  member )
{
	const AEDesc& desc = static_cast< const AEDesc& >( obj );
	
	if ( member == "descriptorType" )
	{
		return desc.get_descriptorType();
	}
	
	if ( member == "dataHandle" )
	{
		return desc.get_dataHandle();
	}
	
	if ( member == "to" )
	{
		return bind_args( Proc( proc_to ), obj );
	}
	
	return NIL;
}

// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||  ::mac::sys::gestalt( 'evnt' ));

const type_info AEDesc_vtype =
{
	"AEDesc",
	apple_events_present ? (assign_proc) &assign_to< AEDesc > : NULL,
	&AEDesc::coerce,
	NULL,
	&AEDesc_member,
	0,
};

}
}
