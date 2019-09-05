/*
	extended-api-set/part-2.h
	-------------------------
	
	Joshua Juran
*/

#ifndef EXTENDEDAPISET_PART2_H
#define EXTENDEDAPISET_PART2_H

#if !defined( __RELIX__ )  &&  !defined( __linux__ )  &&  !defined( __CYGWIN__ )

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif

// POSIX
// Include <sys/types.h> before <dirent.h> to compile with Mac OS X 10.2's SDK.
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAC_OS_X_VERSION_10_10

#if __FreeBSD__ < 8  // undefined __FreeBSD__ counts as zero
#if !(__OpenBSD__ && AT_FDCWD)

/*
	gcc 4.0.1 build 5247 (PPC) has no O_DIRECTORY.  Define it to match
	build 5367 on Intel.
*/

#ifndef O_DIRECTORY
#ifdef __APPLE__
#define O_DIRECTORY  0x100000
#else
#define O_DIRECTORY  0
#endif
#endif

#define AT_FDCWD  (-100)

#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR  1
#endif

#ifndef AT_SYMLINK_FOLLOW
#define AT_SYMLINK_FOLLOW  2
#endif

#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW  4096
#endif

// dirent
DIR *fdopendir( int fd );

// fcntl
#ifdef __cplusplus
int openat( int dirfd, const char* path, int flags, mode_t mode = 0 );
#else
int openat( int dirfd, const char* path, int flags, ... );
#endif

// stdio
int renameat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath );

// sys/stat
int fstatat( int dirfd, const char* path, struct stat* sb, int flags );
int mkdirat( int dirfd, const char* path, mode_t mode );
int mkfifoat( int dirfd, const char* path, mode_t mode );
int mknodat( int dirfd, const char* path, mode_t mode, dev_t dev );

// time
int futimesat( int dirfd, const char* path, const struct timeval times[2] );

// unistd
int faccessat( int dirfd, const char* path, int mode, int flags );
int fchmodat( int dirfd, const char* path, mode_t mode, int flags );
int fchownat( int dirfd, const char* path, uid_t owner, gid_t group, int flags );
int fexecve( int fd, char *const argv[], char *const envp[] );
int linkat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags );
ssize_t readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size );
int symlinkat( const char* target_path, int newdirfd, const char* newpath );
int unlinkat( int dirfd, const char* path, int flags );

#endif  // #if !(__OpenBSD__ && AT_FDCWD)
#endif  // #if __FreeBSD__ < 8
#endif  // #ifndef MAC_OS_X_VERSION_10_10

int futimens( int fd, const struct timespec times[2] );

#ifdef __cplusplus
}
#endif

#endif  // #if !defined( __RELIX__ )  &&  !defined( __linux__ )  &&  !defined( __CYGWIN__ )

#endif
