/*
	rot13.c
	-------
*/

// POSIX
#include <unistd.h>


#define READ_ERROR   "rot13: read error\n"
#define WRITE_ERROR  "rot13: write error\n"

int main()
{
	char buffer[ 1024 ];
	
	ssize_t n_read;
	
	while ( (n_read = read( STDIN_FILENO, buffer, sizeof buffer )) > 0 )
	{
		const char* end = buffer + n_read;
		
		char* p = buffer;
		
		for ( ;  p < end;  ++p )
		{
			if ( (*p | 0x20) >= 'a'  &&  (*p | 0x20) <= 'z' )
			{
				*p += (*p | 0x20) < 'n' ? 13 : -13;
			}
		}
		
		if ( write( STDOUT_FILENO, buffer, n_read ) != n_read )
		{
			write( STDERR_FILENO, WRITE_ERROR, sizeof WRITE_ERROR - 1 );
			
			return 1;
		}
	}
	
	if ( n_read < 0 )
	{
		write( STDERR_FILENO, READ_ERROR, sizeof READ_ERROR - 1 );
		
		return 1;
	}
	
	return 0;
}

