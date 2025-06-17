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

// AESendBlocking
#include "AESendBlocking.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"
#include "varyx/mac/OSErr.hh"


namespace varyx
{
namespace mac
{

static
Value v_AESend( const Value& v )
{
	const AEDesc& desc = static_cast< const AEDesc& >( v );
	
	AEDesc result;
	
	::AppleEvent const* event = &desc.get();
	::AppleEvent*       reply = result.pointer();
	
	OSErr err = ::AESendBlocking( event, reply );
	
	throw_MacOS_error( err, "AESend" );
	
	return result;
}

static const Type AESend( AEDesc_vtype );

const proc_info proc_AESend = { "AESend", &v_AESend, &AESend };

}
}
