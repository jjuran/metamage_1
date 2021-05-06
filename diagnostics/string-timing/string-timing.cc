/*
	string-timing.cc
	----------------
*/

// Standard C
#include <signal.h>
#include <stdint.h>
#include <stdio.h>

// Standard C++
#include <string>

// iota
#include "iota/strings.hh"

// compat
#include "clock/time.h"

// plus
#include "plus/cow_string.hh"
#include "plus/own_string.hh"

#ifdef __MWERKS__
#pragma once off
#endif


static const std::string& move( const std::string& s )
{
	return s;
}

class skipped_test {};

#define SKIP  throw skipped_test()


static uint64_t microclock()
{
	timespec ts;
	
	clock_gettime( CLOCK_MONOTONIC, &ts );
	
	return ts.tv_sec * 1000000ull + ts.tv_nsec / 1000;
}

#ifdef __RELIX__
#define microclock() clock()
#endif

#ifdef __MACOS__
#define K 1
#else
#define K 100
#endif

const int n_trials = 7;

static int i;


class timer
{
	private:
		const char* const its_name;
		
		const uint64_t its_start;
	
	public:
		timer( const char* name ) : its_name( name ), its_start( microclock() )
		{
		}
		
		uint64_t get() const
		{
			return microclock() - its_start;
		}
		
	#ifdef __RELIX__
		
		~timer() { kill( 1, 0 ); }  // guaranteed yield point in MacRelix
		
	#endif
};

const char* test_names[] =
{
	"default ctor x2",
	"small   ctor   ",
	"normal  ctor   ",
	"small    copy  ",
	"sharable copy  ",
	"static   copy  ",
	"const iterator ",
	"(COW) iterator ",
	"unsharable copy",
	"unsharable move",
	"small    move  ",
	"sharable move  ",
	"moved iterator ",
	""
};

int main( int argc, char **argv )
{
	//printf( "%s\n", STRING );
	
	const int n = K * 1000;
	
	#undef I
	#define I 0
	#include "run-test.hh"
	
	#undef I
	#define I 1
	#include "run-test.hh"
	
	#undef I
	#define I 2
	#include "run-test.hh"
	
	#undef I
	#define I 3
	#include "run-test.hh"
	
	#undef I
	#define I 4
	#include "run-test.hh"
	
	#ifdef PLUS_STRING
	#undef I
	#define I 5
	#include "run-test.hh"
	#endif
	
	#undef I
	#define I 6
	#include "run-test.hh"
	
	#undef I
	#define I 7
	#include "run-test.hh"
	
	#undef I
	#define I 8
	#include "run-test.hh"
	
	#undef I
	#define I 9
	#include "run-test.hh"
	
	#undef I
	#define I 10
	#include "run-test.hh"
	
	#undef I
	#define I 11
	#include "run-test.hh"
	
	#undef I
	#define I 12
	#include "run-test.hh"
	
	return 0;
}
