/*	================
 *	LinkerOptions.cc
 *	================
 */

#include "A-line/LinkerOptions.hh"

// C++
#include <functional>

// A-line
#include "A-line/TargetNames.hh"


namespace ALine {
	
	// Static variables are zero-initialized
	static FSSpec sNullFSSpec;
	
	LinkerOptions::LinkerOptions( const TargetInfo& targetInfo )
	:
		targetInfo( targetInfo ), 
		myOutputFile( sNullFSSpec )
	{
		
	}
	
	void LinkerOptions::SetOutput( const FSSpec& output )
	{
		myOutputFile = output;
	}
	
}

