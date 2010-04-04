/*
	extended-api-set/part-2.h
	-------------------------
	
	Joshua Juran
*/

#ifndef EXTENDEDAPISET_PART2_H
#define EXTENDEDAPISET_PART2_H

// POSIX
#include <dirent.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LAMP__

#ifndef __linux__

#define AT_FDCWD  (-100)

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
int fexecve( int fd, const char *const argv[], const char *const envp[] );
int linkat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags );
ssize_t readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size );
int symlinkat( const char* target_path, int newdirfd, const char* newpath );
int unlinkat( int dirfd, const char* path, int flags );

#endif  // !__linux__

#endif  // !__LAMP__

#ifdef __cplusplus
}
#endif

#endif

