/*	======
 *	tty.cc
 *	======
 */

// Standard C/C++
#include <cstring>

// POSIX
#include "unistd.h"


#pragma export on

int main( int /*argc*/, char const *const /*argv*/[] )
{
	const char* name = ttyname( STDIN_FILENO );
	
	if ( name != NULL )
	{
		write( STDOUT_FILENO, name, std::strlen( name ) );
		write( STDOUT_FILENO, "\n", 1                   );
	}
	else
	{
		const char noTTY[] = "not a tty\n";
		
		write( STDOUT_FILENO, noTTY, sizeof noTTY - 1 );
	}
	
	return 0;
}

#pragma export reset

