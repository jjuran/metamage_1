/*
	rwperm.cc
	---------
*/

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 4 + 4;


#define TMP_DIR  "/tmp/rwperm-tests-" __TIME__
#define TMP_DIR_  TMP_DIR "/"


static void bootstrap()
{
	CHECK( mkdir( TMP_DIR, 0755 ) );
	
	CHECK( chdir( TMP_DIR ) );
}


static void rdonly()
{
	int fd = CHECK( open( "rdonly", O_RDONLY | O_CREAT | O_EXCL, 0644 ) );
	
	char buffer[ 512 ] = { 0 };
	
	EXPECT( read( fd, buffer, sizeof buffer ) == 0 );
	
	EXPECT( write ( fd, buffer, sizeof buffer    ) == -1  &&  errno == EBADF );
	EXPECT( pwrite( fd, buffer, sizeof buffer, 0 ) == -1  &&  errno == EBADF );
	
	EXPECT( ftruncate( fd, 0 ) == -1  &&  (errno == EBADF  ||  errno == EINVAL) );
	
	CHECK( close( fd ) );
}

static void wronly()
{
	int fd = CHECK( open( "wronly", O_WRONLY | O_CREAT | O_EXCL, 0644 ) );
	
	char buffer[ 4096 ] = { 0 };
	
	EXPECT( read( fd, buffer, sizeof buffer ) == -1  &&  errno == EBADF );
	
	EXPECT( write ( fd, buffer, sizeof buffer    ) == sizeof buffer );
	EXPECT( pwrite( fd, buffer, sizeof buffer, 0 ) == sizeof buffer );
	
	EXPECT( ftruncate( fd, 0 ) == 0 );
	
	CHECK( close( fd ) );
}


static void cleanup()
{
	(void) unlink( "rdonly" );
	(void) unlink( "wronly" );
	
	(void) rmdir( TMP_DIR );
}


int main( int argc, char** argv )
{
	tap::start( "rwperm", n_tests );
	
	bootstrap();
	
	rdonly();
	wronly();
	
	cleanup();
	
	return 0;
}
