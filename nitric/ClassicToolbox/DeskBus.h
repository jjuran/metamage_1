/*	=========
 *	DeskBus.h
 *	=========
 */

#ifndef CLASSICTOOLBOX_DESKBUS_H
#define CLASSICTOOLBOX_DESKBUS_H

#if TARGET_API_MAC_CARBON
#error This module is for classic Toolbox only
#endif

// Mac OS Universal Interfaces
#ifndef __DESKBUS__
#include <DeskBus.h>
#endif

// Nucleus
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif

// Nitrogen
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif


namespace Nitrogen
{
	
	class ADBManagerErrorsRegistrationDependency
	{
		public:
			ADBManagerErrorsRegistrationDependency();
	};
	
	typedef Nucleus::ID< struct ADBAddress_Tag, ADBAddress >::Type ADBAddress;
	
	// ProcPtrs and UPPs
	
	struct ADBCompletionUPP_Details : Basic_UPP_Details< ::ADBCompletionUPP,
	                                                     ::ADBCompletionProcPtr,
	                                                     ::NewADBCompletionUPP,
	                                                     ::DisposeADBCompletionUPP,
	                                                     ::InvokeADBCompletionUPP >
	{};
	
	typedef UPP< ADBCompletionUPP_Details > ADBCompletionUPP;
	
	using ::ADBDataBlock;
	using ::ADBSetInfoBlock;
	using ::ADBOpBlock;
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	using ::ADBReInit;
	
	void ADBOp( ::Ptr refCon, ADBCompletionUPP completion, ::Ptr buffer, short commandNum );
	
	using ::CountADBs;
	
	class GetIndADB_Failed {};
	
	ADBAddress GetIndADB( ADBDataBlock& data, short index );
	
	ADBDataBlock GetADBInfo( ADBAddress adbAttr );
	
	void SetADBInfo( const ADBSetInfoBlock& info, ADBAddress adbAttr );
	
	// UPP management
	
	inline Nucleus::Owned< ADBCompletionUPP > NewADBCompletionUPP( ::ADBCompletionProcPtr p )
	{
		return NewUPP< ADBCompletionUPP >( p );
	}
	
	inline void DisposeADBCompletionUPP( Nucleus::Owned< ADBCompletionUPP > )  {}
	
	inline void InvokeADBCompletionUPP( Ptr               buffer,
	                                    Ptr               refCon,
	                                    long              command,
	                                    ADBCompletionUPP  userUPP )
	{
		userUPP( buffer, refCon, command );
	}
	
}

#endif

