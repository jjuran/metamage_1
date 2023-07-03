/*
	RdWr_OpenResFile_Scope.cc
	-------------------------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/Utilities/RdWr_OpenResFile_Scope.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-rsrc-utils
#include "mac_rsrc/open_res_file.hh"

// Nitrogen
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif


#define TopMapHndl  (*(Handle*) 0x0A50)


namespace Genie
{
	
#if TARGET_API_MAC_CARBON
	
	typedef ::ResFileRefNum  TopOfResourceChain;
	
#else
	
	typedef ::Handle         TopOfResourceChain;
	
#endif
	
	
	static inline
	::ResFileRefNum GetTopResourceFile()
	{
		::ResFileRefNum topRefNum = -1;
		
		Mac::ThrowOSStatus( ::GetTopResourceFile( &topRefNum ) );
		
		return topRefNum;
	}
	
	static inline TopOfResourceChain GetTop()
	{
	#if TARGET_API_MAC_CARBON
		
		return GetTopResourceFile();
		
	#else
		
		return TopMapHndl;
		
	#endif
	}
	
	RdWr_OpenResFile_Scope::RdWr_OpenResFile_Scope( const FSSpec& resFile )
	:
		itsPreviouslyCurrentResFile( ::CurResFile() )
	{
		const TopOfResourceChain top = GetTop();
		
		short opened = mac::rsrc::open_res_file( resFile, fsRdWrPerm );
		
		if ( opened < 0 )
		{
			Mac::ThrowOSStatus( opened );
		}
		
		itsResFile = opened;
		
		const bool map_reused = GetTop() == top;
		
		if ( map_reused )
		{
			// This is the weird condition in which a previously returned
			// resource file reference number will be again returned for a
			// subsequent open call.
			
			// New refnums become current, but old ones do not:
			::UseResFile( itsResFile );
			
			// Zero the refnum so we don't close it at end of scope
			itsResFile = 0;
		}
	}
	
	RdWr_OpenResFile_Scope::~RdWr_OpenResFile_Scope()
	{
		if ( itsResFile )
		{
			::CloseResFile( itsResFile );
		}
		
		::UseResFile( itsPreviouslyCurrentResFile );
	}
	
}
