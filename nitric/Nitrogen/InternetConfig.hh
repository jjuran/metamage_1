// Nitrogen/InternetConfig.hh
// --------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_INTERNETCONFIG_HH
#define NITROGEN_INTERNETCONFIG_HH

#ifndef __INTERNETCONFIG__
#include <InternetConfig.h>
#endif

#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
#include "Nitrogen/MacTypes.hh"
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( InternetConfig );
	
	using ::ICInstance;
	using ::ICDirSpec;
	
	enum ICAttr
	{
		kICAttrNoChange     = ::kICAttrNoChange,
		kICAttrLockedMask   = ::kICAttrLockedMask,
		kICAttrVolatileMask = ::kICAttrVolatileMask,
		
		kICAttr_Max = nucleus::enumeration_traits< ::ICAttr >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( ICAttr )
	
	enum ICPerm
	{
		icNoPerm        = ::icNoPerm,
		icReadOnlyPerm  = ::icReadOnlyPerm,
		icReadWritePerm = ::icReadWritePerm,
		
		kICPerm_Max = nucleus::enumeration_traits< ::ICPerm >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( ICPerm )
	
	enum ICProfileID
	{
		kICNilProfileID = ::kICNilProfileID,
		
		kICProfileID_Max = nucleus::enumeration_traits< ::ICProfileID >::max
	};
	
	static const OSType kICFileType = OSType( ::kICFileType );
	static const OSType kICCreator  = OSType( ::kICCreator  );
	
	static const AEEventClass kInternetEventClass = AEEventClass( ::kInternetEventClass );
	
	static const AEEventID kAEGetURL   = AEEventID( ::kAEGetURL   );
	static const AEEventID kAEFetchURL = AEEventID( ::kAEFetchURL );
	
	static const AEKeyword keyAEAttaching = AEKeyword( ::keyAEAttaching );
	
	static const AEEventClass kICEditPreferenceEventClass = AEEventClass( ::kICEditPreferenceEventClass );
	
	static const AEEventID kICEditPreferenceEvent = AEEventID( ::kICEditPreferenceEvent );
	
	static const AEKeyword keyICEditPreferenceDestination = AEKeyword( ::keyICEditPreferenceDestination );
	
	using ::ICFontRecordHandle;
	using ::ICCharTableHandle;
	using ::ICAppSpecHandle;
	using ::ICAppSpecListHandle;
	using ::ICFileSpecHandle;
	using ::ICMapEntryHandle;
	using ::ICServiceEntryHandle;
	using ::ICServicesHandle;
}

namespace nucleus
{
	
	template <> struct disposer< Nitrogen::ICInstance > : public std::unary_function< Nitrogen::ICInstance, void >,
	                                                      private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::ICInstance instance ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::InternetConfig );
			
			HandleDestructionOSStatus( ::ICStop( instance ) );
		}
	};
	
	template <>  struct disposer_traits< Nitrogen::ICFontRecordHandle   > : disposer_traits< Nitrogen::Handle >  {};
	template <>  struct disposer_traits< Nitrogen::ICCharTableHandle    > : disposer_traits< Nitrogen::Handle >  {};
	template <>  struct disposer_traits< Nitrogen::ICAppSpecHandle      > : disposer_traits< Nitrogen::Handle >  {};
	template <>  struct disposer_traits< Nitrogen::ICAppSpecListHandle  > : disposer_traits< Nitrogen::Handle >  {};
	template <>  struct disposer_traits< Nitrogen::ICFileSpecHandle     > : disposer_traits< Nitrogen::Handle >  {};
	template <>  struct disposer_traits< Nitrogen::ICMapEntryHandle     > : disposer_traits< Nitrogen::Handle >  {};
	template <>  struct disposer_traits< Nitrogen::ICServiceEntryHandle > : disposer_traits< Nitrogen::Handle >  {};
	template <>  struct disposer_traits< Nitrogen::ICServicesHandle     > : disposer_traits< Nitrogen::Handle >  {};
	
}

namespace Nitrogen
{
	
	nucleus::owned< ICInstance > ICStart( OSType signature );
	
	void ICStop( nucleus::owned< ICInstance >& instance );
	
	UInt32 ICGetVersion( ICInstance  instance,
	                     long        whichVersion );
	
	Str255 ICGetConfigName( ICInstance instance, bool longname );
	
	long ICGetSeed( ICInstance instance );
	
	ICPerm ICGetPerm( ICInstance instance );
	
	// ICBegin
	// ICGetPref
	// ICSetPref
	
	// 1179
	nucleus::owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                           ConstStr255Param  key,
	                                           ICAttr&           attr );
	
	nucleus::owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                           ConstStr255Param  key );
	
	// ICGetPrefHandle
	// ICSetPrefHandle
	
	long ICCountPref( ICInstance instance );
	
	// ICGetIndPref
	// ICDeletePref
	// ICEnd
	// ICGetDefaultPref
	
	// ICEditPreferences
	// ICLaunchURL
	// ICParseURL
	// ICCreateGURLEvent
	// ICSendGURLEvent
	
	// ICMapFilename
	// ICMapTypeCreator
	// ICMapEntriesFilename
	// ICMapEntriesTypeCreator
	
	// ICCountMapEntries
	// ICGetIndMapEntry
	// ICGetMapEntry
	// ICSetMapEntry
	// ICDeleteMapEntry
	// ICAddMapEntry
	
	// ICGetCurrentProfile
	// ICSetCurrentProfile
	// ICCountProfiles
	// ICGetIndProfile
	// ICGetProfileName
	// ICSetProfileName
	// ICAddProfile
	// ICDeleteProfile
	
}

#endif

