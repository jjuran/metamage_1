/*
	RdWr_OpenResFile_Scope.hh
	-------------------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef GENIE_UTILITIES_RDWROPENRESFILESCOPE_HH
#define GENIE_UTILITIES_RDWROPENRESFILESCOPE_HH

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif


namespace Genie
{
	
	class RdWr_OpenResFile_Scope
	{
		private:
			::ResFileRefNum     itsPreviouslyCurrentResFile;
			::ResFileRefNum     itsResFile;
		
		private:
			// Non-copyable
			RdWr_OpenResFile_Scope           ( const RdWr_OpenResFile_Scope& );
			RdWr_OpenResFile_Scope& operator=( const RdWr_OpenResFile_Scope& );
		
		public:
			RdWr_OpenResFile_Scope( const FSSpec& resFile );
			
			~RdWr_OpenResFile_Scope();
	};
	
}

#endif

