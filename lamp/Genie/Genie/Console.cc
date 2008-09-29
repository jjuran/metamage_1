/*	================
 *	Genie/Console.cc
 *	================
 */

#include "Genie/Console.hh"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
	static void SpawnNewConsole( const char* programPath )
	{
		Process& parent = GetInitProcess();
		
		Process* external = new Process( parent );
		
		char const *const argv[] = { programPath, NULL };
		
		(void) external->Exec( programPath, argv, NULL );
		
		parent.ResumeAfterFork();
	}
	
	void SpawnNewConsole()
	{
		return SpawnNewConsole( "/bin/jgetty" );
	}
	
}

