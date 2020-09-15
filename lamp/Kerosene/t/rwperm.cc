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


static const unsigned n_tests = 4 + 4 + 2;


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

static void mmapped()
{
	int fd = CHECK( open( "wronly", O_RDONLY, 0 ) );
	
	void* mem = mmap( NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
	
	EXPECT( mem == MAP_FAILED  &&  errno == EACCES );
	
	if ( mem != MAP_FAILED )
	{
		CHECK( munmap( mem, 4096 ) );
	}
	
	CHECK( close( fd ) );
	
	fd = CHECK( open( "wronly", O_WRONLY, 0 ) );
	
	mem = mmap( NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
	
	EXPECT( mem == MAP_FAILED  &&  errno == EACCES );
	
	if ( mem != MAP_FAILED )
	{
		CHECK( munmap( mem, 4096 ) );
	}
	
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
	
	mmapped();
	
	cleanup();
	
	return 0;
}
