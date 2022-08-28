/*
	ProcessSerialNumber.cc
	----------------------
*/

#include "varyx/mac/ProcessSerialNumber.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#ifndef MAC_OS_X_VERSION_10_4
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// vlib
#include "vlib/types/integer/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/struct/type.hh"


namespace varyx
{
namespace mac
{

const Struct_Type& struct_ProcessSerialNumber()
{
	static Struct_Type type
#if defined( __GNUC__ )  &&  __GNUC__ < 4
	= Struct_Type
#endif
	(
		Value( Value( String( "highLongOfPSN" ), Op_mapping, u32_type() ),
			   Value( String( "lowLongOfPSN"  ), Op_mapping, u32_type() ) ),
		String( "ProcessSerialNumber" )
	);
	
	return type;
}

static inline
bool is_PSN_type( const Struct_Type& type )
{
	return type.expr() == struct_ProcessSerialNumber().expr();
}

bool ProcessSerialNumber::test( const Value& v )
{
	const Struct* s = v.is< Struct >();
	
	return s  &&  is_PSN_type( s->get_type() );
}

ProcessSerialNumber::ProcessSerialNumber()
:
	Struct( struct_ProcessSerialNumber() )
{
}

ProcessSerialNumber::ProcessSerialNumber( const ::ProcessSerialNumber& psn )
:
	Struct( struct_ProcessSerialNumber() )
{
	get_data().pod_cast< ::ProcessSerialNumber >() = psn;
}

}
}
