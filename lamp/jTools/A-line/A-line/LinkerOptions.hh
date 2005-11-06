/*	================
 *	LinkerOptions.hh
 *	================
 */

#pragma once

#include <TargetConditionals.h>
#if TARGET_RT_MAC_MACHO
	#include <Carbon/Carbon.h>
#else
	#include <Files.h>
#endif

// C++
#include <map>
#include <string>
#include <vector>

// A-line
#include "A-line/TargetTypes.hh"


namespace ALine {
	
	using std::string;
	using std::vector;
	
	
	struct TargetInfo;
	
	class LinkerOptions
	{
		private:
			const TargetInfo& targetInfo;
			FSSpec myOutputFile;
			vector< FSSpec > myObjectFiles;
		
		public:
			LinkerOptions( const TargetInfo& targetInfo );
			
			const TargetInfo& Target() const  { return targetInfo; }
			
			void SetOutput( const FSSpec& output );
			
			const FSSpec& Output() const  { return myOutputFile; }
			
			vector< FSSpec > const& ObjectFiles() const  { return myObjectFiles; }
			vector< FSSpec >      & ObjectFiles()        { return myObjectFiles; }
			
	};
	
}

