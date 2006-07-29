/*	========
 *	unistd.h
 *	========
 */

#ifndef KEROSENE_UNISTD_H
#define KEROSENE_UNISTD_H

#include <stddef.h>

#include "sys/time.h"
#include "sys/types.h"
#include "vfork.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	#define STDIN_FILENO    0       /* Standard input.  */
	#define STDOUT_FILENO   1       /* Standard output.  */
	#define STDERR_FILENO   2       /* Standard error output.  */
	
	extern char** environ;
	
	unsigned int alarm( unsigned int seconds );
	
	int chdir( const char* path );
	
	int chown( const char* path, uid_t owner, gid_t group );
	
	int close( int fd );
	
	int copyfile( const char* src, const char* dest );
	
	int dup ( int oldfd );
	int dup2( int oldfd, int newfd );
	
	int execl ( const char* path, const char* arg, ... );
	int execle( const char* path, const char* arg, ... );
	int execlp( const char* file, const char* arg, ... );
	
	int execv ( const char* path, const char* const argv[] );
	int execve( const char* path, const char* const argv[], const char* const envp[] );
	int execvp( const char* file, const char* const argv[] );
	
	int execve_Kernel( const char* path, const char* const argv[], const char* const envp[] );
	
	void _exit( int status );
	void _exit_Kernel( int status );
	
	int fork();
	
	int ftruncate(int fd, off_t length);
	int truncate(const char *path, off_t length);
	
	char* getcwd( char* buf, size_t size );
	
	long gethostid();
	int sethostid( long id );
	
	int gethostname( char *name, size_t len );
	int sethostname( const char *name, size_t len );
	
	gid_t getgid();
	gid_t getegid();
	
	uid_t getuid();
	uid_t geteuid();
	
	pid_t getpgid( pid_t pid );
	pid_t getpgrp();
	pid_t getpid();
	pid_t getppid();
	
	int isatty(int desc);
	
	off_t lseek( int fildes, off_t offset, int whence );
	
	int pause();
	
	int peek( int fd, const char** buffer, size_t minBytes );
	
	int pipe( int filedes[ 2 ] );
	
	ssize_t read( int fd, void* buf, size_t count );
	
	int readlink(const char *path, char *buf, size_t bufsiz);
	
	int rename( const char* src, const char* dest );
	
	int rmdir(const char *pathname);
	
	// setctty() makes fd the controlling terminal of the current process.
	// I made this up.
	int setctty( int fd );
	
	int setpgid( pid_t pid, pid_t pgid );
	int setpgrp();
	
	pid_t setsid();
	
	int setgid( gid_t gid );
	int setuid( uid_t uid );
	
	unsigned int sleep( unsigned int seconds );
	
	int symlink(const char *oldpath, const char *newpath);
	
	const char* ttyname( int filedes );
	
	// ttypair() is analogous to socketpair(), and creates a pseudo-tty device.
	// File descriptors refering to the master and slave respectively are returned
	// in filedes.
	// I made this up too.
	int ttypair( int filedes[ 2 ] );
	
	int unlink( const char *pathname );
	
	ssize_t write( int fd, const void* buf, size_t count );
	
#ifdef __cplusplus
}
#endif

#endif

