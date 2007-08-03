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
	
	
	void SpawnNewConsole( const FSSpec& program )
	{
		// Create new console/terminal device
		// Spawn new process with file descriptors set
		
		Process& parent = GetProcess( 1 );
		
		Process* external = new Process( parent );
		
		std::string programName = NN::Convert< std::string >( program.name );
		
		char const *const argv[] = { programName.c_str(), NULL };
		
		(void) external->Exec( program, argv, NULL );
	}
	
	static FSSpec LoginExecutable()
	{
		return ResolvePathname( "/bin/jgetty", FSTreePtr() )->GetFSSpec();
	}
	
	void SpawnNewConsole()
	{
		return SpawnNewConsole( LoginExecutable() );
	}
	
}

