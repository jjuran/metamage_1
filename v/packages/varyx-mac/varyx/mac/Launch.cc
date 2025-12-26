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
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/types/null.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"
#include "varyx/mac/FSRef.hh"
#include "varyx/mac/FSSpec.hh"
#include "varyx/mac/OSErr.hh"
#include "varyx/mac/ProcessSerialNumber.hh"


namespace varyx
{
namespace mac
{

static
Value v_LaunchApplication( const Value& v )
{
	OSErr err;
	
	void* appParams = NULL;
	
	const Value& file = first( v );
	const Value& aevt = rest ( v );
	
#if defined( __APPLE__ )  &&  ! __LP64__
	
	if ( FSRef::test( file ) )
	{
		return v_LaunchApplication( Value( FSSpec::coerce( file ), aevt ) );
	}
	
#endif
	
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
	pb.launchAppParameters	= (AppParameters*) appParams;
	
#if __LP64__
	
	const ::FSRef& ref = static_cast< const FSRef& >( file ).get();
	
	pb.launchAppRef = const_cast< ::FSRef* >( &ref );
	
#else 
	
	const ::FSSpec& spec = static_cast< const FSSpec& >( file ).get();
	
	pb.launchAppSpec = const_cast< ::FSSpec* >( &spec );
	
#endif
	
	err = LaunchApplication( &pb );
	
	AEDisposeDesc( &coerced );
	
	throw_MacOS_error( err, "LaunchApplication" );
	
	return ProcessSerialNumber( pb.launchProcessSN );
}

#ifdef __APPLE__
static const Type fsref  = FSRef_vtype;
#endif

#if ! __LP64__
static const Type fsspec = FSSpec_vtype;
#endif

static const Type aedesc = AEDesc_vtype;

static const Null null;

#if __LP64__
	#define FILEDESC fsref
#elif defined( __APPLE__ )
	static const Value fsref_or_fsspec( fsref, Op_union, fsspec );
	#define FILEDESC fsref_or_fsspec
#else
	#define FILEDESC fsspec
#endif

static const Value aedesc_or_null ( aedesc,         Op_union,     null );
static const Value optional_aedesc( aedesc_or_null, Op_duplicate, null );

static const Value proto_LaunchApplication( FILEDESC, optional_aedesc );

const proc_info proc_LaunchApplication =
{
	"LaunchApplication",
	&v_LaunchApplication,
	&proto_LaunchApplication,
};

}
}
