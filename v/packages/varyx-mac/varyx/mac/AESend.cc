/*
	AESend.cc
	---------
*/

#include "varyx/mac/AESend.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif
#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"


namespace varyx
{
namespace mac
{

static
Value v_AESend( const Value& v )
{
	const AEDesc& desc = static_cast< const AEDesc& >( v );
	
	::AppleEvent const* event = &desc.get();
	::AppleEvent*       reply = NULL;
	
	const AESendMode     mode     = kAENoReply;
	const AESendPriority priority = kAENormalPriority;
	const SInt32         timeout  = kAEDefaultTimeout;
	
	OSErr err = ::AESend( event, reply, mode, priority, timeout, NULL, NULL );
	
	throw_MacOS_error( err, "AESend" );
	
	return Value_nothing;
}

static const Type AESend( AEDesc_vtype );

const proc_info proc_AESend = { "AESend", &v_AESend, &AESend };

}
}
