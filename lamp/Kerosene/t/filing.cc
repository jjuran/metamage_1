/*
	filing.cc
	---------
*/

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

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


static const unsigned n_tests = 5 + 6 + 7 + 4 + 20 + 13 + 4;


using tap::ok_if;


#define TMP_DIR  "/tmp/filing-tests-" __TIME__
#define TMP_DIR_  TMP_DIR "/"


static bool exists( const char* path )
{
	struct ::stat stat_buffer;
	
	return lstat( path, &stat_buffer ) == 0;
}

static bool is_reg( const char* path )
{
	struct ::stat sb;
	
	return lstat( path, &sb ) == 0  &&  S_ISREG( sb.st_mode );
}

static bool is_lnk( const char* path )
{
	struct ::stat sb;
	
	return lstat( path, &sb ) == 0  &&  S_ISLNK( sb.st_mode );
}

static bool is_dir( const char* path )
{
	struct ::stat sb;
	
	return lstat( path, &sb ) == 0  &&  S_ISDIR( sb.st_mode );
}

static bool resolves( const char* path )
{
	struct ::stat stat_buffer;
	
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
	struct ::stat root_stat;
	struct ::stat cwd_stat;
	struct ::stat parent_stat;
	struct ::stat tmp_stat;
	struct ::stat tmpdir_stat;
	
	CHECK( lstat( "/",  &root_stat   ) );
	CHECK( lstat( ".",  &cwd_stat    ) );
	CHECK( lstat( "..", &parent_stat ) );
	
	CHECK( stat( "/tmp", &tmp_stat ) );
	
	ok_if( S_ISDIR( root_stat  .st_mode ) );
	ok_if( S_ISDIR( cwd_stat   .st_mode ) );
	ok_if( S_ISDIR( parent_stat.st_mode ) );
	ok_if( S_ISDIR( tmp_stat   .st_mode ) );
	
	devnull_fd = CHECK( open( "/dev/null", O_RDWR, 0 ) );
	
	CHECK( mkdir( TMP_DIR, 0755 ) );
	
	CHECK( chdir( TMP_DIR ) );
	
	CHECK( lstat( TMP_DIR, &tmpdir_stat ) );
	
	const bool tmpdir_is_dir = S_ISDIR( tmpdir_stat.st_mode );
	
	ok_if( tmpdir_is_dir );
	
	if ( !tmpdir_is_dir )
	{
		exit( 0 );
	}
	
	CHECK( symlink( "loop-de-loop", "loop-de-loop" ) );
}


static void create_rename_unlink()
{
	ok_if( !exists( "foo" ) );
	ok_if( !exists( "bar" ) );
	
	create( "foo" );
	
	ok_if( is_reg( "foo" ) );
	
	CHECK( rename( "foo", "bar" ) );
	
	ok_if( !exists( "foo" ) );
	
	ok_if( is_reg( "bar" ) );
	
	CHECK( unlink( "bar" ) );
	
	ok_if( !exists( "bar" ) );
}

static void mkdir_errs()
{
	ok_if( mkdir( TMP_DIR_,            0755 ) == -1  &&  errno == EEXIST  );
	ok_if( mkdir( "loop-de-loop/foo",  0755 ) == -1  &&  errno == ELOOP   );
	ok_if( mkdir( "foo/foo",           0755 ) == -1  &&  errno == ENOENT  );
	ok_if( mkdir( "",                  0755 ) == -1  &&  errno == ENOENT  );
	ok_if( mkdir( "/bin/sh/foo",       0755 ) == -1  &&  errno == ENOTDIR );
	
	ok_if( mkdirat( -1,         "foo", 0755 ) == -1  &&  errno == EBADF   );
	ok_if( mkdirat( devnull_fd, "foo", 0755 ) == -1  &&  errno == ENOTDIR );
}

static void t_mkdirat()
{
	CHECK( mkdirat( AT_FDCWD, "foo", 0755 ) );
	
	ok_if( is_dir( "foo" ) );
	
	CHECK( mkdirat( devnull_fd, TMP_DIR_ "bar", 0755 ) );
	
	ok_if( is_dir( "bar" ) );
	
	int foo_fd = CHECK( open( "foo", O_RDONLY | O_DIRECTORY, 0 ) );
	
	CHECK( mkdirat( foo_fd, TMP_DIR_ "baz", 0755 ) );
	
	ok_if( is_dir( "baz" ) );
	
	CHECK( mkdirat( foo_fd, "bar", 0755 ) );
	
	ok_if( is_dir( "foo/bar" ) );
	
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
	
	struct ::stat sb;
	
	CHECK( mkdir( "foo", 0755 ) );
	
	ok_if( readlink( "foo",              buffer, sizeof buffer ) == -1  &&  errno == EINVAL  );
	ok_if( readlink( "loop-de-loop/foo", buffer, sizeof buffer ) == -1  &&  errno == ELOOP   );
	ok_if( readlink( "bar/foo",          buffer, sizeof buffer ) == -1  &&  errno == ENOENT  );
	ok_if( readlink( "bar",              buffer, sizeof buffer ) == -1  &&  errno == ENOENT, "readlink( non ) -> ENOENT"  );
	ok_if( readlink( "/dev/foo",         buffer, sizeof buffer ) == -1  &&  errno == ENOENT, "readlink( non ) -> ENOENT"  );
	ok_if( readlink( "",                 buffer, sizeof buffer ) == -1  &&  errno == ENOENT  );
	ok_if( readlink( "/bin/sh/foo",      buffer, sizeof buffer ) == -1  &&  errno == ENOTDIR );
	
	n = CHECK( readlink( "loop-de-loop", buffer, sizeof buffer ) );
	
	ok_if( EQUAL( buffer, n, "loop-de-loop" ) );
	
	ok_if( symlink( "_", "foo"          ) == -1  &&  errno == EEXIST );
	ok_if( symlink( "_", "loop-de-loop" ) == -1  &&  errno == EEXIST );
	ok_if( symlink( "_", "/dev/null"    ) == -1  &&  errno == EEXIST );
	
	CHECK( symlinkat( "1", AT_FDCWD, "one" ) );
	
	CHECK( fstatat( AT_FDCWD, TMP_DIR_ "one", &sb, AT_SYMLINK_NOFOLLOW ) );
	
	ok_if( S_ISLNK( sb.st_mode ) );
	
	ok_if( fstatat( AT_FDCWD, TMP_DIR_ "one", &sb, 0 ) == -1  &&  errno == ENOENT );
	
	n = CHECK( readlinkat( -1, TMP_DIR_ "one", buffer, sizeof buffer ) );
	
	ok_if( EQUAL( buffer, n, "1" ) );
	
	ok_if( unlinkat( AT_FDCWD, "one", AT_REMOVEDIR ) == -1  &&  errno == ENOTDIR );
	
	ok_if( unlinkat( AT_FDCWD, "one", 0 ) == 0 );
	
	ok_if( fstatat( AT_FDCWD, TMP_DIR_ "one", &sb, AT_SYMLINK_NOFOLLOW ) == -1  &&  errno == ENOENT );
	
	ok_if( unlinkat( AT_FDCWD, "foo", 0 ) == -1  &&  errno == EPERM, "unlinkat( dir ) -> EPERM" );
	
	ok_if( unlinkat( AT_FDCWD, "foo", AT_REMOVEDIR ) == 0 );
	
	ok_if( !exists( "foo" ) );
	
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
	
	ok_if( rename( "a", "foo" ) == -1  &&  errno == EISDIR  );
	ok_if( rename( "foo", "a" ) == -1  &&  errno == ENOTDIR );
	
	ok_if( rename( "a", "a" ) == 0 );
	
	ok_if( exists( "a" ) );
	
	ok_if( renameat( AT_FDCWD, "a", foo_fd, "a" ) == 0, "renameat( AT_FDCWD, x, dirfd, x )" );
	
	ok_if( exists( "foo/a" ) );
	
	(void) rename( "a", "foo/a" );
	
	ok_if( renameat( foo_fd, "a", bar_fd, "a" ) == 0, "renameat( dirfd, x, dirfd, x )" );
	
	ok_if( exists( "bar/a" ) );
	
	(void) rename( "foo/a", "bar/a" );
	
	ok_if( rename( "foo", "bar" ) == -1  &&  (errno == EEXIST || errno == ENOTEMPTY) );
	
	ok_if( rename( "bar", "foo" ) == 0 );
	
	ok_if( exists( "foo" )  &&  !exists( "bar" ) );
	
	ok_if( unlinkat( foo_fd, "a", 0 ) == -1  &&  errno == ENOENT );
	
	ok_if( unlinkat( bar_fd, "a", 0 ) == 0 );
	
	close( foo_fd );
	close( bar_fd );
	
	(void) rmdir( "foo" );
	(void) rmdir( "bar" );
}

static void linkat_t()
{
#ifdef __RELIX__
	
	// skipped
	ok_if( true );
	ok_if( true );
	ok_if( true );
	ok_if( true );
	
	return;
	
#endif
	
	create( "foo" );
	
	CHECK( symlink( "foo", "bar" ) );
	
	int linked = linkat( AT_FDCWD, "bar", AT_FDCWD, "baz", 0 );
	
	if ( linked == 0 )
	{
		ok_if( true, "linkat() links symlinks" );
		
		ok_if( is_lnk( "baz" ) );
		
		(void) unlink( "baz" );
	}
	else
	{
		ok_if( errno == ENOSYS, "linkat() doesn't link symlinks" );
		
		ok_if( true );  // skipped
	}
	
	ok_if( linkat( AT_FDCWD, "bar", AT_FDCWD, "baz", AT_SYMLINK_FOLLOW ) == 0 );
	
	ok_if( is_reg( "baz" ) );
	
	(void) unlink( "baz" );
	(void) unlink( "bar" );
	(void) unlink( "foo" );
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
	
	cleanup();
	
	return 0;
}

