/*	================
 *	Genie/Console.cc
 *	================
 */

#include "Genie/Console.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
	static void SpawnNewConsole( const FSTreePtr& program )
	{
		// Create new console/terminal device
		// Spawn new process with file descriptors set
		
		Process& parent = GetProcess( 1 );
		
		Process* external = new Process( parent );
		
		std::string programName = program->Name();
		
		char const *const argv[] = { programName.c_str(), NULL };
		
		(void) external->Exec( program, argv, NULL );
	}
	
	void SpawnNewConsole( const FSSpec& program )
	{
		return SpawnNewConsole( FSTreeFromFSSpec( program ) );
	}
	
	static FSTreePtr LoginExecutable()
	{
		return ResolvePathname( "/bin/jgetty", FSTreePtr() );
	}
	
	void SpawnNewConsole()
	{
		return SpawnNewConsole( LoginExecutable() );
	}
	
}

