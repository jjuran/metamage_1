// Nitrogen/UPP.hh
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_UPP_HH
#define NITROGEN_UPP_HH

// Nitrogen
#ifndef MAC_MIXEDMODE_TYPES_UPP_HH
#include "Mac/MixedMode/Types/UPP.hh"
#endif


namespace Nitrogen
{
	
	using Mac::Basic_UPP_Details;
	using Mac::NewUPP;
	using Mac::UPP;
	using Mac::GlueUPP;
	using Mac::StaticUPP;
	using Mac::StaticUPPWorkaround;
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	using Mac::Call_With_A0_Glue;
	
#endif
	
}

#endif

