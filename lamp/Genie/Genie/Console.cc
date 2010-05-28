/*	================
 *	Genie/Console.cc
 *	================
 */

#include "Genie/Console.hh"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	static void SpawnNewConsole( const char* programPath )
	{
		Process& parent = GetInitProcess();
		
		const boost::intrusive_ptr< Process >& external = NewProcess( parent );
		
		char const *const argv[] = { programPath, NULL };
		
		try
		{
			(void) external->Exec( programPath, argv, NULL );
		}
		catch ( ... )
		{
			GetProcessList().RemoveProcess( external->GetPID() );
		}
		
		parent.ResumeAfterFork();
	}
	
	void SpawnNewConsole()
	{
		return SpawnNewConsole( "/bin/jgetty" );
	}
	
}

