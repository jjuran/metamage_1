/*	================
 *	Genie/Console.hh
 *	================
 */

#ifndef GENIE_CONSOLE_HH
#define GENIE_CONSOLE_HH


struct FSSpec;


namespace Genie
{
	
	void SpawnNewConsole( const FSSpec& program );
	void SpawnNewConsole();
	
}

#endif

