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

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "sys/wait.h"
#include "unistd.h"

// BSD
#include <vfork.h>

// Kerosene
#include "SystemCalls.hh"


#pragma export on
extern "C"
{

	void (*_exit_Ptr)(int);
	
}
#pragma export reset


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
	
	//#pragma mark -
	#pragma mark ¥ dirent ¥
	
	int closedir( DIR* dir )
	{
		return -1;
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
		std::string name = ttyname( fd );
		
		const char tty[] = "/dev/tty";
		
		return name.substr( 0, sizeof tty - 1 ) == tty;
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
		
		int result = execve_Stub( path, argv, envp );
		
		if ( result != -1 && gKerosene_vforking )
		{
			ExitFromFork( pid );
		}
		
		return result;
	}
	
	void _exit( int status )
	{
		pid_t pid = getpid();
		
		if ( _exit_Ptr )
		{
			_exit_Ptr( status );
		}
		
		if ( gKerosene_vforking )
		{
			ExitFromFork( pid );  // doesn't return -- jumps back to vfork()
		}
	}
	
