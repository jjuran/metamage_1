// Nitrogen/ControlDefinitions.cc
// ------------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/ControlDefinitions.hh"


namespace Nitrogen
{
	
	namespace Tests
	{
		
		static void Test_kControlEditTextCFStringTag( ControlRef control, CFStringRef string )
		{
			Nitrogen::SetControlData< kControlEditTextCFStringTag >( control, kControlNoPart, string );
		}
		
	}
	
}

