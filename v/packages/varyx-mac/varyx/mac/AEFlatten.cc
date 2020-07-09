/*
	AEFlatten.cc
	------------
*/

#include "varyx/mac/AEFlatten.hh"

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

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"


namespace varyx
{
namespace mac
{

using namespace vlib;

static
Value v_AEFlatten( const Value& v )
{
	const AEDesc& desc = static_cast< const AEDesc& >( v );
	
	Size size = AESizeOfFlattenedDesc( &desc.get() );
	
	plus::string result;
	
	char* p = result.reset( size );
	
	OSStatus err = AEFlattenDesc( &desc.get(), p, size, &size );
	
	throw_MacOS_error( err, "AEFlattenDesc" );
	
	return Packed( result );
}

static const Type aedesc = AEDesc_vtype;

const bool has_AEFlattenDesc =
	TARGET_API_MAC_OSX  ||  (TARGET_API_MAC_CARBON  &&  &AEFlattenDesc != NULL);

const proc_info proc_AEFlatten =
{
	"AEFlatten",
	has_AEFlattenDesc ? &v_AEFlatten : NULL,
	&aedesc,
};

}
}
