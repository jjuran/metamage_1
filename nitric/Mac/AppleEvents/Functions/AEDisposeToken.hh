/*
	Mac/AppleEvents/Functions/AEDisposeToken.hh
	-------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSETOKEN_HH
#define MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSETOKEN_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEOBJECTS__
#include <AEObjects.h>
#endif

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEDESC_HH
#include "Mac/AppleEvents/Types/AEDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_UTILITIES_NONNULLAEDESCSARELIVE_HH
#include "Mac/AppleEvents/Utilities/NonNull_AEDescs_Are_Live.hh"
#endif


namespace nucleus
{
	
	template <>
	struct disposer< ::Mac::AEDesc_Token >
	{
		typedef AEDesc  argument_type;
		typedef void    result_type;
		
		// parameter can't be const
		void operator()( AEDesc desc ) const
		{
			::AEDisposeToken( &desc );
		}
	};
	
	template <>
	struct aliveness_traits< ::Mac::AEDesc_Token, disposer< ::Mac::AEDesc_Token > >
	{
		typedef ::Mac::NonNull_AEDescs_Are_Live aliveness_test;
	};
	
	template <>
	struct null_resource< ::Mac::AEDesc_Token >
	{
		static const ::Mac::AEDesc_Token& value;
		
		static const ::Mac::AEDesc_Token& get()  { return value; }
	};
	
}

namespace Mac
{
	
	inline void AEDisposeToken( nucleus::owned< AEDesc_Token > )
	{
	}
	
}

#endif

