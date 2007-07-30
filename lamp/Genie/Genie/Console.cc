/*	================
 *	Genie/Console.cc
 *	================
 */

#include "Genie/Console.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/ConsoleTTY.hh"
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
		
		FileDescriptorMap& files = external->FileDescriptors();
		
		// Temporary IORef in nested block goes out of scope prior to Exec().
		// This is necessary because an unforked exec() will lose temporaries.
		{
			boost::shared_ptr< IOHandle > terminal = NewConsoleDevice();
			
			files[ 0 ] = terminal;
			files[ 1 ] = terminal;
			files[ 2 ] = terminal;
		}
		
		std::string programName = NN::Convert< std::string >( program.name );
		
		char const *const argv[] = { programName.c_str(), NULL };
		
		(void) external->Exec( program, argv, NULL );
	}
	
	static FSSpec LoginExecutable()
	{
		return ResolvePathname( "/bin/login", FSTreePtr() )->GetFSSpec();
	}
	
	void SpawnNewConsole()
	{
		return SpawnNewConsole( LoginExecutable() );
	}
	
}

