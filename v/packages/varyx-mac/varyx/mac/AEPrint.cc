/*
	AEPrint.cc
	----------
*/

#include "varyx/mac/AEPrint.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#ifndef __AEHELPERS__
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

// Mac OS
#ifndef __AEHELPERS__
#include <AEHelpers.h>
#endif

// Standard C
#include <string.h>

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// plus
#include "plus/mac_utf8.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"


namespace varyx
{
namespace mac
{

using namespace vlib;

static
Value v_AEPrint( const Value& v )
{
	OSStatus err;
	
	const AEDesc& desc = static_cast< const AEDesc& >( v );
	
	Handle handle;
	err = AEPrintDescToHandle( &desc.get(), &handle );
	
	throw_MacOS_error( err, "AEPrintDescToHandle" );
	
	const Size size = GetHandleSize( handle ) - 1;  // drop trailing NUL byte
	
	plus::string result;
	
	char* p = result.reset( size );
	
	memcpy( p, *handle, size );
	
	DisposeHandle( handle );
	
	return String( plus::utf8_from_mac( result ) );
}

static const Type aedesc = AEDesc_vtype;

// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||  ::mac::sys::gestalt( 'evnt' ));

const proc_info proc_AEPrint =
{
	"AEPrint",
	apple_events_present ? &v_AEPrint : NULL,
	&aedesc,
};

}
}
