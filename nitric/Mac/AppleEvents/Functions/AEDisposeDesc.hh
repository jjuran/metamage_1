/*
	Mac/AppleEvents/Functions/AEDisposeDesc.hh
	------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSEDESC_HH
#define MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSEDESC_HH

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
	struct disposer< ::Mac::AEDesc_Data >
	{
		typedef AEDesc  argument_type;
		typedef void    result_type;
		
		// parameter can't be const
		void operator()( AEDesc desc ) const
		{
			::AEDisposeDesc( &desc );
		}
	};
	
	template <>
	struct aliveness_traits< ::Mac::AEDesc_Data, disposer< ::Mac::AEDesc_Data > >
	{
		typedef ::Mac::NonNull_AEDescs_Are_Live aliveness_test;
	};
	
	template <>
	struct null_resource< ::Mac::AEDesc_Data >
	{
		static const ::Mac::AEDesc_Data& value;
		
		static const ::Mac::AEDesc_Data& get()  { return value; }
	};
	
}

namespace Mac
{
	
	inline void AEDisposeDesc( nucleus::owned< AEDesc_Data > )
	{
	}
	
}

#endif

