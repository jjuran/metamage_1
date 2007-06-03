// Nucleus/Flag.cp
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif


namespace Nucleus
{
	
	namespace Testing
	{
		
		enum Foo
		{
			foo0,
			foo1,
			foo2
		};
		
		NUCLEUS_DEFINE_FLAG_OPS( Foo )
		
		static void TestFlagOps()
		{
			Foo bitUnion = foo1 | foo2;
		}
		
	}
	
}

