/*
	AboutBox.cc
	-----------
*/

#include "Genie/AboutBox.hh"

// Genie
#include "Genie/ProcessList.hh"


namespace Genie
{
	
	void ShowAboutBox()
	{
		spawn_process( "/sbin/about" );
	}
	
}

