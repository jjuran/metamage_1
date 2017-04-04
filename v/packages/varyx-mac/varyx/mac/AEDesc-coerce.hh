/*
	AEDesc-coerce.hh
	----------------
*/

#ifndef VARYX_MAC_AEDESCCOERCE_HH
#define VARYX_MAC_AEDESCCOERCE_HH

// varyx-mac
#include "varyx/mac/AEDesc.hh"


namespace varyx
{
namespace mac
{
	
	using namespace vlib;
	
	AEDesc coerce_to_AEDesc( const Value& v );
	
}
}

#endif
