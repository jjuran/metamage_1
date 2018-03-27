/*
	queues.mac.cc
	-------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 3 + 7 + 17 + 16;


static QHdr testQueue;

static QElem e1;
static QElem e2;
static QElem e3;

static
void empty()
{
	EXPECT( Dequeue( &e1, &testQueue ) == qErr );
	
	EXPECT( testQueue.qHead == NULL );
	EXPECT( testQueue.qTail == NULL );
}

static
void one()
{
	e1.qLink = &e1;
	
	Enqueue( &e1, &testQueue );
	
	EXPECT( testQueue.qHead == &e1 );
	EXPECT( testQueue.qTail == &e1 );
	
	EXPECT( e1.qLink == NULL );
	
	EXPECT( Dequeue( &e2, &testQueue ) == qErr );
	
	EXPECT( Dequeue( &e1, &testQueue ) == noErr );
	
	EXPECT( testQueue.qHead == NULL );
	EXPECT( testQueue.qTail == NULL );
}

static
void two()
{
	Enqueue( &e1, &testQueue );
	Enqueue( &e2, &testQueue );
	
	EXPECT( testQueue.qHead == &e1 );
	EXPECT( testQueue.qTail == &e2 );
	
	EXPECT( e1.qLink == &e2 );
	EXPECT( e2.qLink == NULL );
	
	EXPECT( Dequeue( &e3, &testQueue ) == qErr );
	
	EXPECT( Dequeue( &e1, &testQueue ) == noErr );
	
	EXPECT( testQueue.qHead == &e2 );
	EXPECT( testQueue.qTail == &e2 );
	
	EXPECT( e2.qLink == NULL );
	
	EXPECT( Dequeue( &e1, &testQueue ) == qErr );
	
	EXPECT( Dequeue( &e2, &testQueue ) == noErr );
	
	EXPECT( testQueue.qHead == NULL );
	EXPECT( testQueue.qTail == NULL );
	
	Enqueue( &e1, &testQueue );
	Enqueue( &e2, &testQueue );
	
	EXPECT( Dequeue( &e2, &testQueue ) == noErr );
	
	EXPECT( testQueue.qHead == &e1 );
	EXPECT( testQueue.qTail == &e1 );
	
	EXPECT( e1.qLink == NULL );
}

static
void setup_three()
{
	testQueue.qHead = NULL;
	testQueue.qTail = NULL;
	
	Enqueue( &e1, &testQueue );
	Enqueue( &e2, &testQueue );
	Enqueue( &e3, &testQueue );
}

static
void three()
{
	setup_three();
	
	EXPECT( testQueue.qHead == &e1 );
	EXPECT( testQueue.qTail == &e3 );
	
	EXPECT( e1.qLink == &e2 );
	EXPECT( e2.qLink == &e3 );
	EXPECT( e3.qLink == NULL );
	
	EXPECT( Dequeue( &e1, &testQueue ) == noErr );
	
	EXPECT( testQueue.qHead == &e2 );
	EXPECT( testQueue.qTail == &e3 );
	
	setup_three();
	
	EXPECT( Dequeue( &e2, &testQueue ) == noErr );
	
	EXPECT( testQueue.qHead == &e1 );
	EXPECT( testQueue.qTail == &e3 );
	
	EXPECT( e1.qLink == &e3 );
	
	setup_three();
	
	EXPECT( Dequeue( &e3, &testQueue ) == noErr );
	
	EXPECT( testQueue.qHead == &e1 );
	EXPECT( testQueue.qTail == &e2 );
	
	EXPECT( e2.qLink == NULL );
}

int main( int argc, char** argv )
{
	tap::start( "queues.mac", n_tests );
	
	empty();
	one();
	two();
	three();
	
	return 0;
}
