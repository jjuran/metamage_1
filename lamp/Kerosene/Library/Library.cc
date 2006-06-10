/*	==========
 *	Library.cc
 *	==========
 */

// Standard C
#include "errno.h"
#include <stdarg.h>
#include "stdlib.h"
#include "string.h"

// Standard C++
#include <string>
#include <vector>

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "sys/ioctl.h"
#include "sys/wait.h"
#include "unistd.h"

// BSD
#include <vfork.h>

// Kerosene
#include "SystemCalls.hh"


	const char* sys_errlist[] =
	{
		"No error",
		"Operation not permitted",
		"No such file or directory",
		"No such process",
		"Interrupted system call",
		"Input/output error",
		"Device not configured",
		"Argument list too long",
		"Exec format error",
		"Bad file descriptor",
		"No child processes",
		"Resource deadlock avoided",
		"Cannot allocate memory",
		"Permission denied",
		"Bad address",
		"Block device required",
		"Device busy",
		"File exists",
		"Cross-device link",
		"Operation not supported by device",
		"Not a directory",
		"Is a directory",
		"Invalid argument",
		"Too many open files in system",
		"Too many open files",
		"Inappropriate ioctl for device",
		"Text file busy",
		"File too large",
		"No space left on device",
		"Illegal seek",
		"Read-only file system",
		"Too many links",
		"Broken pipe",
		"Numerical argument out of domain",
		"Result too large",
		"Operation would block",
		"Operation now in progress",
		"Operation already in progress",
		"Socket operation on non-socket",
		"Destination address required",
		"Message too long",
		"Protocol wrong type for socket",
		"Protocol not available",
		"Protocol not supported",
		"Socket type not supported",
		"Operation not supported on socket",
		"Protocol family not supported",
		"Address family not supported by protocol family",
		"Address already in use",
		"Can't assign requested address",
		"Network is down",
		"Network is unreachable",
		"Network dropped connection on reset",
		"Software caused connection abort",
		"Connection reset by peer",
		"No buffer space available",
		"Socket is already connected",
		"Socket is not connected",
		"Can't send after socket shutdown",
		"Too many references: can't splice",
		"Connection timed out",
		"Connection refused",
		"Too many levels of symbolic links",
		"File name too long",
		"Host is down",
		"No route to host",
		"Directory not empty",
		"Too many processes",
		"Too many users",
		"Disc quota exceeded",
		"Stale NFS file handle",
		"Too many levels of remote in path",
		"RPC struct is bad",
		"RPC version wrong",
		"RPC prog. not avail",
		"Program version wrong",
		"Bad procedure for program",
		"No locks available",
		"Function not implemented",
		"Inappropriate file type or format",
		NULL
	};
	
	// Length of array (not counting trailing NULL)
	int sys_nerr = sizeof sys_errlist / sizeof (const char*) - 1;
	
	extern "C" char* strerror( int errnum );
	
	char* strerror( int errnum )
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
	
	int system( const char* command )
	{
		int pid = vfork();
		
		if ( pid == 0 )
		{
			const char* argv[] = { "sh", "-c", command, NULL };
			
			execv( "/bin/sh", (char**)argv );
			
			_exit( 1 );
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
	
	int setctty( int fd )
	{
		return ioctl( fd, TIOCSCTTY, NULL );
	}
	
	int setpgrp()
	{
		return setpgid( 0, 0 );
	}
	
	FSSpec CurrentDirectory()
	{
		return Path2FSS( "." );
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
	
