/*
	Launch.cc
	---------
*/

#include "varyx/mac/Launch.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"
#include "varyx/mac/FSSpec.hh"


namespace varyx
{
namespace mac
{

#if ! __LP64__

static
Value v_LaunchApplication( const Value& v )
{
	OSErr err;
	
	const ::FSSpec& spec = static_cast< const FSSpec& >( v ).get();
	
	LaunchParamBlockRec pb;
	
	pb.reserved1			= 0;
	pb.reserved2			= 0;
	pb.launchBlockID 		= extendedBlock;
	pb.launchEPBLength 		= extendedBlockLen;
	pb.launchFileFlags 		= 0;
	pb.launchControlFlags	= launchContinue | launchNoFileFlags;
	pb.launchAppSpec 		= const_cast< ::FSSpec* >( &spec );
	pb.launchAppParameters	= NULL;
	
	err = LaunchApplication( &pb );
	
	throw_MacOS_error( err, "LaunchApplication" );
	
	return nothing;
}

static const Type fsspec = FSSpec_vtype;

const proc_info proc_LaunchApplication =
{
	"LaunchApplication",
	&v_LaunchApplication,
	&fsspec,
};

#else

int dummy;

#endif  // #if ! __LP64__

}
}
