/*

Possible ways to compile tests

static function -- tests are compiled, but not linked or run
static invocation -- a file-static object runs the test in its constructor
static registration -- a file-static object registers the test in its constructor
	* flexible testing
		* tests can be run or not (without recompiling)
		* tests can be run interactively, chosen and initiated by user
		* tests can be sorted by file modification date.
		
	

*/

#include "Tests.hh"

// Standard C
#include <stdio.h>

// Standard C++
#include <algorithm>
#include <vector>

// Debug
#include "debug/assert.hh"


namespace Test
{
	
	static TesterBase* gTester = NULL;
	
	static std::vector< const Group* >& Groups()
	{
		static std::vector< const Group* > gTests;
		
		return gTests;
	}
	
	void TesterBase::RunAll()
	{
		gTester = this;
		
		/*
		// This code slices *this*
		std::for_each( Groups().begin(),
		               Groups().end(),
		               *this );
		*/
		
		typedef std::vector< const Group* >::const_iterator const_iterator;
		
		for ( const_iterator it = Groups().begin();  it != Groups().end();  ++it )
		{
			Run( *it );
		}
		
		gTester = NULL;
	}
	
	void TesterBase::Run( const Group* group )
	{
		ASSERT( group != NULL );
		
		group->testProc();
	}
	
	void TesterBase::Assert( bool condition, const char* test, const char* file, int line )
	{
		if ( condition )
		{
			Passed( test, file, line );
		}
		else
		{
			Failed( test, file, line );
		}
	}
	
	void Assert( bool condition, const char* test, const char* file, int line )
	{
		if ( gTester )
		{
			gTester->Assert( condition, test, file, line );
		}
	}
	
	
	static void Register( const Group* group )
	{
		ASSERT( group != NULL );
		
		Groups().push_back( group );
	}
	
	Group::Group( TestProcPtr testProc, const char* label,
	                                    const char* date,
	                                    const char* time,
	                                    const char* file, int line ) : testProc( testProc ),
	                                                                   label   ( label    ),
	                                                                   date    ( date     ),
	                                                                   time    ( time     ),
	                                                                   file    ( file     ),
	                                                                   line    ( line     )
	{
		Register( this );
	}
	
	void SampleTester::Run( const Group* group )
	{
		ASSERT( group != NULL );
		
		printf( "Testing case %s\n", group->label );
		
		fResults.push_back( GroupResult( *group ) );
		
		TesterBase::Run( group );
		
		printf( "\n" );
	}
	
	void SampleTester::Assert( bool condition, const char* test, const char* file, int line )
	{
		bool verbose = true;
		
		if ( verbose )
		{
			printf( "%s: %s\n", (condition ? "PASSED" : "FAILED"), test );
		}
		
		fResults.back().fResults.push_back( Result( condition ? kTestPassed
		                                                      : kTestFailed, test ) );
		
	}
	
}


namespace Module
{
	
	static int add( int a, int b )
	{
		return a + b;
	}
	
	
	TEST( 2005-03-03 04:45:00 )
	{
		TEST_ASSERT( add( 2, 2 ) == 4 );
	}
	
}
