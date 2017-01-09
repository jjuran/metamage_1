/*
	filing.cc
	---------
*/

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// iota
#include "iota/strings.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 5 + 6 + 7 + 4 + 20 + 13 + 4 + 1;


#define TMP_DIR  "/tmp/filing-tests-" __TIME__
#define TMP_DIR_  TMP_DIR "/"


static bool exists( const char* path )
{
	struct stat stat_buffer;
	
	return lstat( path, &stat_buffer ) == 0;
}

static bool is_reg( const char* path )
{
	struct stat sb;
	
	return lstat( path, &sb ) == 0  &&  S_ISREG( sb.st_mode );
}

static bool is_lnk( const char* path )
{
	struct stat sb;
	
	return lstat( path, &sb ) == 0  &&  S_ISLNK( sb.st_mode );
}

static bool is_dir( const char* path )
{
	struct stat sb;
	
	return lstat( path, &sb ) == 0  &&  S_ISDIR( sb.st_mode );
}

static bool resolves( const char* path )
{
	struct stat stat_buffer;
	
	return stat( path, &stat_buffer ) == 0;
}

static void create( const char* path )
{
	int fd = CHECK( open( path, O_WRONLY | O_CREAT | O_EXCL, 0644 ) );
	
	close( fd );
}

static int devnull_fd = -1;

static void bootstrap()
{
	struct stat root_stat;
	struct stat cwd_stat;
	struct stat parent_stat;
	struct stat tmp_stat;
	struct stat tmpdir_stat;
	
	CHECK( lstat( "/",  &root_stat   ) );
	CHECK( lstat( ".",  &cwd_stat    ) );
	CHECK( lstat( "..", &parent_stat ) );
	
	CHECK( stat( "/tmp", &tmp_stat ) );
	
	EXPECT( S_ISDIR( root_stat  .st_mode ) );
	EXPECT( S_ISDIR( cwd_stat   .st_mode ) );
	EXPECT( S_ISDIR( parent_stat.st_mode ) );
	EXPECT( S_ISDIR( tmp_stat   .st_mode ) );
	
	devnull_fd = CHECK( open( "/dev/null", O_RDWR, 0 ) );
	
	CHECK( mkdir( TMP_DIR, 0755 ) );
	
	CHECK( chdir( TMP_DIR ) );
	
	CHECK( lstat( TMP_DIR, &tmpdir_stat ) );
	
	const bool tmpdir_is_dir = S_ISDIR( tmpdir_stat.st_mode );
	
	EXPECT( tmpdir_is_dir );
	
	if ( !tmpdir_is_dir )
	{
		exit( 0 );
	}
	
	CHECK( symlink( "loop-de-loop", "loop-de-loop" ) );
}


static void create_rename_unlink()
{
	EXPECT( !exists( "foo" ) );
	EXPECT( !exists( "bar" ) );
	
	create( "foo" );
	
	EXPECT( is_reg( "foo" ) );
	
	CHECK( rename( "foo", "bar" ) );
	
	EXPECT( !exists( "foo" ) );
	
	EXPECT( is_reg( "bar" ) );
	
	CHECK( unlink( "bar" ) );
	
	EXPECT( !exists( "bar" ) );
}

static void mkdir_errs()
{
	EXPECT( mkdir( TMP_DIR_,            0755 ) == -1  &&  errno == EEXIST  );
	EXPECT( mkdir( "loop-de-loop/foo",  0755 ) == -1  &&  errno == ELOOP   );
	EXPECT( mkdir( "foo/foo",           0755 ) == -1  &&  errno == ENOENT  );
	EXPECT( mkdir( "",                  0755 ) == -1  &&  errno == ENOENT  );
	EXPECT( mkdir( "/bin/sh/foo",       0755 ) == -1  &&  errno == ENOTDIR );
	
	EXPECT( mkdirat( -1,         "foo", 0755 ) == -1  &&  errno == EBADF   );
	EXPECT( mkdirat( devnull_fd, "foo", 0755 ) == -1  &&  errno == ENOTDIR );
}

static void t_mkdirat()
{
	CHECK( mkdirat( AT_FDCWD, "foo", 0755 ) );
	
	EXPECT( is_dir( "foo" ) );
	
	CHECK( mkdirat( devnull_fd, TMP_DIR_ "bar", 0755 ) );
	
	EXPECT( is_dir( "bar" ) );
	
	int foo_fd = CHECK( open( "foo", O_RDONLY | O_DIRECTORY, 0 ) );
	
	CHECK( mkdirat( foo_fd, TMP_DIR_ "baz", 0755 ) );
	
	EXPECT( is_dir( "baz" ) );
	
	CHECK( mkdirat( foo_fd, "bar", 0755 ) );
	
	EXPECT( is_dir( "foo/bar" ) );
	
	close( foo_fd );
	
	(void) rmdir( "foo/bar" );
	(void) rmdir( "foo" );
	(void) rmdir( "bar" );
	(void) rmdir( "baz" );
}

#define EQUAL( p, n, s )  ( n == STRLEN( s )  &&  memcmp( p, STR_LEN( s ) ) == 0 )

static void symlink_readlink_unlink()
{
	char buffer[ 4096 ];
	
	ssize_t n = 0;
	
	struct stat sb;
	
	CHECK( mkdir( "foo", 0755 ) );
	
	EXPECT( readlink( "foo",              buffer, sizeof buffer ) == -1  &&  errno == EINVAL  );
	EXPECT( readlink( "loop-de-loop/foo", buffer, sizeof buffer ) == -1  &&  errno == ELOOP   );
	EXPECT( readlink( "bar/foo",          buffer, sizeof buffer ) == -1  &&  errno == ENOENT  );
	EXPECT( readlink( "bar",              buffer, sizeof buffer ) == -1  &&  errno == ENOENT  );  // readlink( non ) -> ENOENT
	EXPECT( readlink( "/dev/foo",         buffer, sizeof buffer ) == -1  &&  errno == ENOENT  );  // readlink( non ) -> ENOENT
	EXPECT( readlink( "",                 buffer, sizeof buffer ) == -1  &&  errno == ENOENT  );
	EXPECT( readlink( "/bin/sh/foo",      buffer, sizeof buffer ) == -1  &&  errno == ENOTDIR );
	
	n = CHECK( readlink( "loop-de-loop", buffer, sizeof buffer ) );
	
	EXPECT( EQUAL( buffer, n, "loop-de-loop" ) );
	
	EXPECT( symlink( "_", "foo"          ) == -1  &&  errno == EEXIST );
	EXPECT( symlink( "_", "loop-de-loop" ) == -1  &&  errno == EEXIST );
	EXPECT( symlink( "_", "/dev/null"    ) == -1  &&  errno == EEXIST );
	
	CHECK( symlinkat( "1", AT_FDCWD, "one" ) );
	
	CHECK( fstatat( AT_FDCWD, TMP_DIR_ "one", &sb, AT_SYMLINK_NOFOLLOW ) );
	
	EXPECT( S_ISLNK( sb.st_mode ) );
	
	EXPECT( fstatat( AT_FDCWD, TMP_DIR_ "one", &sb, 0 ) == -1  &&  errno == ENOENT );
	
	n = CHECK( readlinkat( -1, TMP_DIR_ "one", buffer, sizeof buffer ) );
	
	EXPECT( EQUAL( buffer, n, "1" ) );
	
	EXPECT( unlinkat( AT_FDCWD, "one", AT_REMOVEDIR ) == -1  &&  errno == ENOTDIR );
	
	EXPECT( unlinkat( AT_FDCWD, "one", 0 ) == 0 );
	
	EXPECT( fstatat( AT_FDCWD, TMP_DIR_ "one", &sb, AT_SYMLINK_NOFOLLOW ) == -1  &&  errno == ENOENT );
	
	EXPECT( unlinkat( AT_FDCWD, "foo", 0 ) == -1  &&  errno == EPERM );  // unlinkat( dir ) -> EPERM
	
	EXPECT( unlinkat( AT_FDCWD, "foo", AT_REMOVEDIR ) == 0 );
	
	EXPECT( !exists( "foo" ) );
	
	(void) unlink( "one" );
	(void) rmdir( "foo" );
}

static void t_renameat()
{
	CHECK( mkdir( "foo", 0755 ) );
	CHECK( mkdir( "bar", 0755 ) );
	
	int foo_fd = CHECK( open( "foo", O_RDONLY | O_DIRECTORY, 0 ) );
	int bar_fd = CHECK( open( "bar", O_RDONLY | O_DIRECTORY, 0 ) );
	
	create( "a" );
	
	EXPECT( rename( "a", "foo" ) == -1  &&  errno == EISDIR  );
	EXPECT( rename( "foo", "a" ) == -1  &&  errno == ENOTDIR );
	
	EXPECT( rename( "a", "a" ) == 0 );
	
	EXPECT( exists( "a" ) );
	
	EXPECT( renameat( AT_FDCWD, "a", foo_fd, "a" ) == 0 );  // renameat( AT_FDCWD, x, dirfd, x )
	
	EXPECT( exists( "foo/a" ) );
	
	(void) rename( "a", "foo/a" );
	
	EXPECT( renameat( foo_fd, "a", bar_fd, "a" ) == 0 );  // renameat( dirfd, x, dirfd, x )
	
	EXPECT( exists( "bar/a" ) );
	
	(void) rename( "foo/a", "bar/a" );
	
	EXPECT( rename( "foo", "bar" ) == -1  &&  (errno == EEXIST || errno == ENOTEMPTY) );
	
	EXPECT( rename( "bar", "foo" ) == 0 );
	
	EXPECT( exists( "foo" )  &&  !exists( "bar" ) );
	
	EXPECT( unlinkat( foo_fd, "a", 0 ) == -1  &&  errno == ENOENT );
	
	EXPECT( unlinkat( bar_fd, "a", 0 ) == 0 );
	
	close( foo_fd );
	close( bar_fd );
	
	(void) rmdir( "foo" );
	(void) rmdir( "bar" );
}

static void linkat_t()
{
#ifdef __RELIX__
	
	// skipped
	EXPECT( true );
	EXPECT( true );
	EXPECT( true );
	EXPECT( true );
	
	return;
	
#endif
	
	create( "foo" );
	
	CHECK( symlink( "foo", "bar" ) );
	
	int linked = linkat( AT_FDCWD, "bar", AT_FDCWD, "baz", 0 );
	
	if ( linked == 0 )
	{
		EXPECT( true );  // linkat() links symlinks
		
		EXPECT( is_lnk( "baz" ) );
		
		(void) unlink( "baz" );
	}
	else
	{
		EXPECT( errno == ENOSYS );  // linkat() doesn't link symlinks
		
		EXPECT( true );  // skipped
	}
	
	EXPECT( linkat( AT_FDCWD, "bar", AT_FDCWD, "baz", AT_SYMLINK_FOLLOW ) == 0 );
	
	EXPECT( is_reg( "baz" ) );
	
	(void) unlink( "baz" );
	(void) unlink( "bar" );
	(void) unlink( "foo" );
}

static void dups()
{
	EXPECT( dup2( devnull_fd, devnull_fd ) == devnull_fd );
}

static void cleanup()
{
	(void) unlink( "loop-de-loop" );
	
	(void) rmdir( TMP_DIR );
}


int main( int argc, char** argv )
{
	tap::start( "filing", n_tests );
	
	bootstrap();
	
	create_rename_unlink();
	
	mkdir_errs();
	
	t_mkdirat();
	
	symlink_readlink_unlink();
	
	t_renameat();
	
	linkat_t();
	
	dups();
	
	cleanup();
	
	return 0;
}
