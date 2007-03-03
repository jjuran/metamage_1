/*	==========
 *	Library.cc
 *	==========
 */

// Standard C
#include "errno.h"
#include "signal.h"
#include <stdarg.h>
#include <stdio.h>
#include "stdlib.h"
#include "string.h"

// Standard C++
#include <string>
#include <vector>

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "sys/wait.h"
#include "unistd.h"

// BSD
#include <vfork.h>

// Kerosene
#include "SystemCalls.hh"


	const char* sys_errlist[] =
	{
/*  0 */	"No error",
/*  1 */	"Operation not permitted",
/*  2 */	"No such file or directory",
/*  3 */	"No such process",
/*  4 */	"Interrupted system call",
/*  5 */	"Input/output error",
/*  6 */	"Device not configured",
/*  7 */	"Argument list too long",
/*  8 */	"Exec format error",
/*  9 */	"Bad file descriptor",
/* 10 */	"No child processes",
/* 11 */	"Resource deadlock avoided",
/* 12 */	"Cannot allocate memory",
/* 13 */	"Permission denied",
/* 14 */	"Bad address",
/* 15 */	"Block device required",
/* 16 */	"Device busy",
/* 17 */	"File exists",
/* 18 */	"Cross-device link",
/* 19 */	"Operation not supported by device",
/* 20 */	"Not a directory",
/* 21 */	"Is a directory",
/* 22 */	"Invalid argument",
/* 23 */	"Too many open files in system",
/* 24 */	"Too many open files",
/* 25 */	"Inappropriate ioctl for device",
/* 26 */	"Text file busy",
/* 27 */	"File too large",
/* 28 */	"No space left on device",
/* 29 */	"Illegal seek",
/* 30 */	"Read-only file system",
/* 31 */	"Too many links",
/* 32 */	"Broken pipe",
/* 33 */	"Numerical argument out of domain",
/* 34 */	"Result too large",
/* 35 */	"Operation would block",
/* 36 */	"Operation now in progress",
/* 37 */	"Operation already in progress",
/* 38 */	"Socket operation on non-socket",
/* 39 */	"Destination address required",
/* 40 */	"Message too long",
/* 41 */	"Protocol wrong type for socket",
/* 42 */	"Protocol not available",
/* 43 */	"Protocol not supported",
/* 44 */	"Socket type not supported",
/* 45 */	"Operation not supported on socket",
/* 46 */	"Protocol family not supported",
/* 47 */	"Address family not supported by protocol family",
/* 48 */	"Address already in use",
/* 49 */	"Can't assign requested address",
/* 50 */	"Network is down",
/* 51 */	"Network is unreachable",
/* 52 */	"Network dropped connection on reset",
/* 53 */	"Software caused connection abort",
/* 54 */	"Connection reset by peer",
/* 55 */	"No buffer space available",
/* 56 */	"Socket is already connected",
/* 57 */	"Socket is not connected",
/* 58 */	"Can't send after socket shutdown",
/* 59 */	"Too many references: can't splice",
/* 60 */	"Connection timed out",
/* 61 */	"Connection refused",
/* 62 */	"Too many levels of symbolic links",
/* 63 */	"File name too long",
/* 64 */	"Host is down",
/* 65 */	"No route to host",
/* 66 */	"Directory not empty",
/* 67 */	"Too many processes",
/* 68 */	"Too many users",
/* 69 */	"Disc quota exceeded",
/* 70 */	"Stale NFS file handle",
/* 71 */	"Too many levels of remote in path",
/* 72 */	"RPC struct is bad",
/* 73 */	"RPC version wrong",
/* 74 */	"RPC prog. not avail",
/* 75 */	"Program version wrong",
/* 76 */	"Bad procedure for program",
/* 77 */	"No locks available",
/* 78 */	"Function not implemented",
/* 79 */	"Inappropriate file type or format",
			NULL
	};
	
	// Length of array (not counting trailing NULL)
	int sys_nerr = sizeof sys_errlist / sizeof (const char*) - 1;
	
	char* std::strerror( int errnum )
	{
		if ( errnum < 0 )
		{
			Errno() = EINVAL;
			return "strerror: errnum is negative";
		}
		
		if ( errnum >= sys_nerr )
		{
			Errno() = EINVAL;
			return "strerror: errnum exceeds sys_nerr";
		}
		
		return const_cast< char* >( sys_errlist[ errnum ] );
	}
	
	void std::perror( const char* s )
	{
		if ( s != NULL )
		{
			std::fprintf( stderr, "%s: ", s );
		}
		
		std::fprintf( stderr, "%s\n", std::strerror( errno ) );
	}

	static std::string LookupPath( const char* filename )
	{
		const char* pathVar = getenv( "PATH" );
		
		if ( pathVar == NULL )
		{
			return "";
		}
		
		const char* pathEnd = pathVar + std::strlen( pathVar );
		
		while ( pathVar < pathEnd )
		{
			const char* separator = std::strchr( pathVar, ':' );
			
			if ( separator == NULL )
			{
				separator = pathEnd;
			}
			
			// Watch out for empty path elements (e.g. "/bin:/sbin:" -- last is empty)
			if ( separator != pathVar )
			{
				std::string dir( pathVar, separator - pathVar );
				
				std::string tryPath = dir + "/" + filename;
				
				const char* foo = separator;
				
				// This seems to trash separator in CFM68K
				int fd = open( tryPath.c_str(), O_RDONLY );
				
				separator = foo;
				
				if ( foo != separator )
				{
					//std::printf( "%s\n", "Warning: foo != separator" );
				}
				
				if ( fd != -1 )
				{
					close( fd );
					
					//std::printf( "%s\n", " ok" );
					
					return tryPath;
				}
				
				//std::printf( "%s\n", " nope" );
			}
			
			// If we're at the end, then this sets pathVar > pathEnd
			pathVar = separator + 1;
		}
		
		return "";
	}
	
	#pragma mark -
	#pragma mark ¥ stdlib ¥
	
	void abort()
	{
		(void) raise( SIGABRT );
	}
	
	int raise( int sig )
	{
		return kill( getpid(), sig );
	}
	
	int system( const char* command )
	{
		if ( command == NULL )
		{
			const bool shellExists = true;
			
			return shellExists;
		}
		
		int pid = vfork();
		
		if ( pid == 0 )
		{
			const char* argv[] = { "sh", "-c", command, NULL };
			
			execv( "/bin/sh", (char**)argv );
			
			const int exitStatusForExecFailure = 127;
			
			_exit( exitStatusForExecFailure );
		}
		
		int stat = -1;
		pid_t result = waitpid( pid, &stat, 0 );
		
		//return result == -1 ? -1 : stat;
		return stat;  // Am I not clever?
	}
	
	#pragma mark -
	#pragma mark ¥ fcntl ¥
	
	int creat( const char* path, mode_t mode )
	{
		return open( path, O_CREAT | O_TRUNC | O_WRONLY, mode );
	}
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	int chown( const char* path, uid_t owner, gid_t group )
	{
		return 0;
	}
	
	int execl( const char* path, const char* arg0, ... )
	{
		va_list va;
		std::vector< const char* > args;
		
		args.push_back( arg0 );
		
		va_start( va, arg0 );
		
		while ( const char* arg = va_arg( va, const char * ) )
		{
			args.push_back( arg );
		}
		
		va_end( va );
		
		args.push_back( NULL );
		
		const char* const* argv = &args[0];
		
		return execv( path, argv );
	}
	
	int execle( const char* path, const char* arg0, ... )
	{
		va_list va;
		std::vector< const char* > args;
		
		args.push_back( arg0 );
		
		va_start( va, arg0 );
		
		while ( const char* arg = va_arg( va, const char * ) )
		{
			args.push_back( arg );
		}
		
		args.push_back( NULL );
		
		const char* const* envp = va_arg( va, const char* const* );
		
		va_end( va );
		
		const char* const* argv = &args[0];
		
		return execve( path, argv, envp );
	}
	
	int execlp( const char* file, const char* arg0, ... )
	{
		
		va_list va;
		std::vector< const char* > args;
		
		args.push_back( arg0 );
		
		va_start( va, arg0 );
		
		while ( const char* arg = va_arg( va, const char * ) )
		{
			args.push_back( arg );
		}
		
		va_end( va );
		
		args.push_back( NULL );
		
		const char* const* argv = &args[0];
		
		return execvp( file, argv );
	}
	
	int execv( const char* path, const char* const argv[] )
	{
		return execve( path, argv, Environ() );
	}
	
	int execvp( const char* file, const char* const argv[] )
	{
		std::string path;
		
		if ( std::strchr( file, '/' ) == NULL )
		{
			path = LookupPath( file );
			
			if ( path == "" )
			{
				Errno() = ENOENT;
				return -1;
			}
			
			file = path.c_str();
		}
		
		return execv( file, argv );
	}
	
	int fork()
	{
		Errno() = EINVAL;
		return -1;
	}
	
	uid_t getuid()   { return 0; }
	uid_t geteuid()  { return 0; }
	
	uid_t getgid()   { return 0; }
	uid_t getegid()  { return 0; }
	
	int isatty( int fd )
	{
		// This is a hack
		const char* tty_name = ttyname( fd );
		
		if ( tty_name == NULL )
		{
			return false;
		}
		
		std::string name = tty_name;
		
		const char tty[] = "/dev/tty";
		
		return name.substr( 0, sizeof tty - 1 ) == tty;
	}
	
	int setpgrp()
	{
		return setpgid( 0, 0 );
	}
	
	#pragma mark -
	#pragma mark ¥ vfork ¥
	
	jmp_buf gKerosene_vfork_jmp_buf;
	
	static bool gKerosene_vforking = false;
	
	static void ExitFromFork( pid_t pid )
	{
		// ASSERT( gKerosene_vforking );
		
		gKerosene_vforking = false;
		longjmp( gKerosene_vfork_jmp_buf, pid );
	}
	
	int Kerosene_SpawnVFork()
	{
		if ( gKerosene_vforking )
		{
			return -1;  // no nested vforking
		}
		
		int result = SpawnVFork();
		
		if ( result != -1 )
		{
			gKerosene_vforking = true;
		}
		
		return result;
	}
	
	int execve( const char* path, const char* const argv[], const char* const* envp )
	{
		pid_t pid = getpid();
		
		int result = execve_Kernel( path, argv, envp );
		
		if ( result != -1 && gKerosene_vforking )
		{
			ExitFromFork( pid );
		}
		
		return result;
	}
	
	void _exit( int status )
	{
		pid_t pid = getpid();
		
		_exit_Kernel( status );
		
		if ( gKerosene_vforking )
		{
			ExitFromFork( pid );  // doesn't return -- jumps back to vfork()
		}
	}
	
