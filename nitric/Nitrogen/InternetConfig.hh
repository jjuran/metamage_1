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

// Nitrogen
#ifndef MAC_FILES_TYPES_FSCREATOR_HH
#include "Mac/Files/Types/FSCreator.hh"
#endif
#ifndef MAC_FILES_TYPES_FSTYPE_HH
#include "Mac/Files/Types/FSType.hh"
#endif

#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
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
	
	static const Mac::FSType     kICFileType = Mac::FSType   ( ::kICFileType );
	static const Mac::FSCreator  kICCreator  = Mac::FSCreator( ::kICCreator  );
	
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
	
	template <> struct disposer< ICInstance >
	{
		typedef ICInstance  argument_type;
		typedef void        result_type;
		
		void operator()( ICInstance instance ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::InternetConfig );
			
			::Nitrogen::HandleDestructionOSStatus( ::ICStop( instance ) );
		}
	};
	
	template <>  struct disposer_class< ICFontRecordHandle   > : disposer_class< Nitrogen::Handle >  {};
	template <>  struct disposer_class< ICCharTableHandle    > : disposer_class< Nitrogen::Handle >  {};
	template <>  struct disposer_class< ICAppSpecHandle      > : disposer_class< Nitrogen::Handle >  {};
	template <>  struct disposer_class< ICAppSpecListHandle  > : disposer_class< Nitrogen::Handle >  {};
	template <>  struct disposer_class< ICFileSpecHandle     > : disposer_class< Nitrogen::Handle >  {};
	template <>  struct disposer_class< ICMapEntryHandle     > : disposer_class< Nitrogen::Handle >  {};
	template <>  struct disposer_class< ICServiceEntryHandle > : disposer_class< Nitrogen::Handle >  {};
	template <>  struct disposer_class< ICServicesHandle     > : disposer_class< Nitrogen::Handle >  {};
	
}

namespace Nitrogen
{
	
	nucleus::owned< ICInstance > ICStart( Mac::FSCreator signature );
	
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

