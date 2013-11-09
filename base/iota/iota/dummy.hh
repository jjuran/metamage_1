/*
	iota/dummy.hh
	-------------
*/

#ifndef IOTA_DUMMY_HH
#define IOTA_DUMMY_HH

#ifdef __APPLE__


namespace iota
{
	
	namespace _
	{
		
		void dummy()
		{
			/*
				* anonymous namespace avoids no-prototype warnings
					(or at least it did with gcc 4.0, but not 4.2 or clang)
				
				* extern linkage silences no-symbol link warnings
			*/
		}
		
	}
	
}

#endif  // #ifdef __APPLE__

#endif

