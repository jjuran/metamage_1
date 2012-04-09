/*
	string-timing.cc
	----------------
*/

// Standard C
#include <signal.h>
#include <stdio.h>
#include <time.h>

// Standard C++
#include <string>

// iota
#include "iota/strings.hh"

// plus
#include "plus/cow_string.hh"
#include "plus/var_string.hh"

#ifdef __MWERKS__
#pragma once off
#endif


static const std::string& move( const std::string& s )
{
	return s;
}

class skipped_test {};

#define SKIP  throw skipped_test()

#ifndef __STDC_INT64__
typedef long long    uint64_t;
#endif

static uint64_t microclock()
{
	struct timespec got;
	clock_gettime(CLOCK_MONOTONIC, &got);
	return uint64_t( got.tv_sec ) * 1000000 + (got.tv_nsec / 1000);
}

#ifdef __MACH__
#define clock_t uint64_t
#define clock() microclock()
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
		
		const clock_t its_start;
	
	public:
		timer( const char* name ) : its_name( name ), its_start( clock() )
		{
		}
		
		clock_t get() const
		{
			return clock() - its_start;
		}
		
		~timer() { kill( 1, 0 ); }  // guaranteed yield point in MacRelix
};

const char* test_names[] =
{
	"default ctor   ",
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

