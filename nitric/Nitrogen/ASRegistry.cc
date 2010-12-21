// Nitrogen/ASRegistry.cc
// ----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.


#include "Nitrogen/ASRegistry.hh"

// Mac OS
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif


namespace Nitrogen
{
	
	nucleus::owned< Mac::AEDesc_Token > MissingValue()
	{
		return AECreateDesc< Mac::typeType, AEDesc_Token >( Mac::DescType( ::cMissingValue ) );
	}
	
}

