/*
	AEDesc-print.hh
	---------------
*/

#ifndef VARYX_MAC_AEDESCPRINT_HH
#define VARYX_MAC_AEDESCPRINT_HH

// plus
#include "plus/string.hh"

// AEDataModel.h
struct AEDesc;


namespace varyx
{
namespace mac
{
	
	plus::string printable_AEDesc( const ::AEDesc& desc );
	
}
}

#endif
