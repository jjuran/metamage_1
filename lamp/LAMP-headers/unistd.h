/*	========
 *	unistd.h
 *	========
 */

#ifndef KEROSENE_UNISTD_H
#define KEROSENE_UNISTD_H

#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STDIN_FILENO    0       /* Standard input.  */
#define STDOUT_FILENO   1       /* Standard output.  */
#define STDERR_FILENO   2       /* Standard error output.  */

extern char** environ;

int chdir( const char* path );

int close( int fd );

int dup ( int oldfd );
int dup2( int oldfd, int newfd );

int execl ( const char* path, const char* arg, ... );
int execle( const char* path, const char* arg, ... );
int execlp( const char* file, const char* arg, ... );

int execv ( const char* path, char* const argv[] );
int execve( const char* path, char* const argv[], char* const envp[] );
int execvp( const char* file, char* const argv[] );

void _exit( int status );

int fork();

inline bool is_child( pid_t )  { return true; }

char* getcwd( char* buf, size_t size );

long gethostid();
int sethostid( long id );

int gethostname( char *name, size_t len );
int sethostname( const char *name, size_t len );

pid_t getpgid( pid_t pid );
pid_t getpgrp();
pid_t getpid();
pid_t getppid();

int pause();

int pipe( int filedes[ 2 ] );

ssize_t read( int fd, void* buf, size_t count );

int setpgid( pid_t pid, pid_t pgid );
int setpgrp();

pid_t setsid();

// setctty() makes fd the controlling terminal of the current process.
// I made this up.
int setctty( int fd );

unsigned int sleep( unsigned int seconds );

const char* ttyname( int filedes );

// ttypair() is analogous to socketpair(), and creates a pseudo-tty device.
// File descriptors refering to the master and slave respectively are returned
// in filedes.
// I made this up too.
int ttypair( int filedes[ 2 ] );

ssize_t write( int fd, const void* buf, size_t count );

#ifdef __cplusplus
}
#endif

#endif

