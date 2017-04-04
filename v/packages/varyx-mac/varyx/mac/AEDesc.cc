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
#include "vlib/targets.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/mb32.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/record.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc-coerce.hh"


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
		
		default:
			break;
	}
	
	return Value();
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
	NULL,
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
	&AEDesc_member,
};

}
}
