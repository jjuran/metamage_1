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
#include "Nitrogen/Resources.h"


namespace Nitrogen
{
	
	static ResFileRefNum GetTopResourceFile()
	{
		::ResFileRefNum topRefNum = -1;
		
		ThrowOSStatus( ::GetTopResourceFile( &topRefNum ) );
		
		return ResFileRefNum( topRefNum );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	RdWr_OpenResFile_Scope::TopOfResourceChain
	//
	RdWr_OpenResFile_Scope::GetTop()
	{
	#if TARGET_API_MAC_CARBON
		
		return N::GetTopResourceFile();
		
	#else
		
		return LMGetTopMapHndl();
		
	#endif
	}
	
	RdWr_OpenResFile_Scope::RdWr_OpenResFile_Scope( const FSSpec& resFile )
	:
		itsPreviouslyCurrentResFile( ::CurResFile() ),
		itsTop( GetTop() ),
		itsResFile( N::FSpOpenResFile( resFile, N::fsRdWrPerm ).release() ),
		itShouldCloseTheResFile( GetTop() != itsTop )
	{
	}
	
	RdWr_OpenResFile_Scope::~RdWr_OpenResFile_Scope()
	{
		if ( itShouldCloseTheResFile )
		{
			::CloseResFile( itsResFile );
		}
		
		::UseResFile( itsPreviouslyCurrentResFile );
	}
	
}

