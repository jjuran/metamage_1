/*
	waits_for_children.hh
	---------------------
*/

#ifndef RELIX_API_WAITSFORCHILDREN_HH
#define RELIX_API_WAITSFORCHILDREN_HH


namespace relix
{
	
	class process;
	
	
	bool waits_for_children( const process& proc );
	
}

#endif
