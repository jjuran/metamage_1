/*
	RdWr_OpenResFile_Scope.cc
	-------------------------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/Utilities/RdWr_OpenResFile_Scope.hh"

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

// Nitrogen
#include "Nitrogen/Resources.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
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
		
		return LMGetTopMapHndl();
		
	#endif
	}
	
	RdWr_OpenResFile_Scope::RdWr_OpenResFile_Scope( const FSSpec& resFile )
	:
		itsPreviouslyCurrentResFile( ::CurResFile() )
	{
		const TopOfResourceChain top = GetTop();
		
		itsResFile = N::FSpOpenResFile( resFile, Mac::fsRdWrPerm ).release();
		
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
