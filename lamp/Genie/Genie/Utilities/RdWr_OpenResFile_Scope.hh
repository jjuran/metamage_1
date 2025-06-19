/*
	RdWr_OpenResFile_Scope.hh
	-------------------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef GENIE_UTILITIES_RDWROPENRESFILESCOPE_HH
#define GENIE_UTILITIES_RDWROPENRESFILESCOPE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// iota
#include "iota/class.hh"


namespace Genie
{
	
	class RdWr_OpenResFile_Scope
	{
		NON_COPYABLE( RdWr_OpenResFile_Scope )
		NO_NEW_DELETE
		
		private:
			::ResFileRefNum     itsPreviouslyCurrentResFile;
			::ResFileRefNum     itsResFile;
		
		public:
			RdWr_OpenResFile_Scope( const FSSpec& resFile );
			
			~RdWr_OpenResFile_Scope();
	};
	
}

#endif
