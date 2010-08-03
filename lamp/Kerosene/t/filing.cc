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

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 5 + 6 + 7 + 4;


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
	
	cleanup();
	
	return 0;
}

