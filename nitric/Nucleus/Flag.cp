// Flag.cp

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
		
		void TestFlagOps()
		{
			Foo bitUnion = foo1 | foo2;
		}
		
	}
	
}

