// Nitrogen/InternetConfig.h
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_INTERNETCONFIG_H
#define NITROGEN_INTERNETCONFIG_H

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __INTERNETCONFIG__
#include FRAMEWORK_HEADER(HIServices,InternetConfig.h)
#endif

#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#include "Nitrogen/MacTypes.h"
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
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
		
		kICAttr_Max = Nucleus::Enumeration_Traits< ::ICAttr >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( ICAttr )
	
	enum ICPerm
	{
		icNoPerm        = ::icNoPerm,
		icReadOnlyPerm  = ::icReadOnlyPerm,
		icReadWritePerm = ::icReadWritePerm,
		
		kICPerm_Max = Nucleus::Enumeration_Traits< ::ICPerm >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( ICPerm )
	
	enum ICProfileID
	{
		kICNilProfileID = ::kICNilProfileID,
		
		kICProfileID_Max = Nucleus::Enumeration_Traits< ::ICProfileID >::max
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

namespace Nucleus
{
	
	template <> struct Disposer< Nitrogen::ICInstance > : public std::unary_function< Nitrogen::ICInstance, void >,
	                                                      private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::ICInstance instance ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::InternetConfig );
			
			HandleDestructionOSStatus( ::ICStop( instance ) );
		}
	};
	
	template <>  struct Disposer_Traits< Nitrogen::ICFontRecordHandle   > : Disposer_Traits< Nitrogen::Handle >  {};
	template <>  struct Disposer_Traits< Nitrogen::ICCharTableHandle    > : Disposer_Traits< Nitrogen::Handle >  {};
	template <>  struct Disposer_Traits< Nitrogen::ICAppSpecHandle      > : Disposer_Traits< Nitrogen::Handle >  {};
	template <>  struct Disposer_Traits< Nitrogen::ICAppSpecListHandle  > : Disposer_Traits< Nitrogen::Handle >  {};
	template <>  struct Disposer_Traits< Nitrogen::ICFileSpecHandle     > : Disposer_Traits< Nitrogen::Handle >  {};
	template <>  struct Disposer_Traits< Nitrogen::ICMapEntryHandle     > : Disposer_Traits< Nitrogen::Handle >  {};
	template <>  struct Disposer_Traits< Nitrogen::ICServiceEntryHandle > : Disposer_Traits< Nitrogen::Handle >  {};
	template <>  struct Disposer_Traits< Nitrogen::ICServicesHandle     > : Disposer_Traits< Nitrogen::Handle >  {};
	
}

namespace Nitrogen
{
	
	Nucleus::Owned< ICInstance > ICStart( OSType signature );
	
	void ICStop( Nucleus::Owned< ICInstance >& instance );
	
	UInt32 ICGetVersion( ICInstance  instance,
	                     long        whichVersion );
	
	Str255 ICGetConfigName( ICInstance instance, bool longname );
	
	long ICGetSeed( ICInstance instance );
	
	ICPerm ICGetPerm( ICInstance instance );
	
	// ICBegin
	// ICGetPref
	// ICSetPref
	
	// 1179
	Nucleus::Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                           ConstStr255Param  key,
	                                           ICAttr&           attr );
	
	Nucleus::Owned< Handle > ICFindPrefHandle( ICInstance        instance,
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

