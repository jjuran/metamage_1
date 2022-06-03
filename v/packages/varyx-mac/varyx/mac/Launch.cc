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
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// Standard C
#include <stdlib.h>

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/null.hh"
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
	
	void* appParams = NULL;
	
	const Value& file = first( v );
	const Value& aevt = rest ( v );
	
	const ::FSSpec& spec = static_cast< const FSSpec& >( file ).get();
	
	::AEDesc coerced;
	
	coerced.dataHandle = NULL;
	
	if ( const AEDesc* desc = aevt.is< AEDesc >() )
	{
		err = AECoerceDesc( &desc->get(), typeAppParameters, &coerced );
		
		throw_MacOS_error( err, "AECoerceDesc" );
		
	#if ! TARGET_API_MAC_CARBON
		
		HLock( coerced.dataHandle );
		
		appParams = *coerced.dataHandle;
		
	#else
		
		Size size = AEGetDescDataSize( &coerced );
		
		appParams = alloca( size );
		
		err = AEGetDescData( &coerced, appParams, size );
		
	#endif
	}
	
	LaunchParamBlockRec pb;
	
	pb.reserved1			= 0;
	pb.reserved2			= 0;
	pb.launchBlockID 		= extendedBlock;
	pb.launchEPBLength 		= extendedBlockLen;
	pb.launchFileFlags 		= 0;
	pb.launchControlFlags	= launchContinue | launchNoFileFlags;
	pb.launchAppSpec 		= const_cast< ::FSSpec* >( &spec );
	pb.launchAppParameters	= (AppParameters*) appParams;
	
	err = LaunchApplication( &pb );
	
	AEDisposeDesc( &coerced );
	
	throw_MacOS_error( err, "LaunchApplication" );
	
	return nothing;
}

static const Type fsspec = FSSpec_vtype;
static const Type aedesc = AEDesc_vtype;

static const Type null = null_vtype;

static const Value aedesc_or_null ( aedesc,         Op_union,     null );
static const Value optional_aedesc( aedesc_or_null, Op_duplicate, null );

static const Value proto_LaunchApplication( fsspec, optional_aedesc );

const proc_info proc_LaunchApplication =
{
	"LaunchApplication",
	&v_LaunchApplication,
	&proto_LaunchApplication,
};

#else

int dummy;

#endif  // #if ! __LP64__

}
}
