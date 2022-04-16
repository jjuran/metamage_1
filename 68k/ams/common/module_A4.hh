/*
	module_A4.hh
	------------
*/

#ifndef MODULEA4_HH
#define MODULEA4_HH

extern unsigned long module_A4;

inline
asm long swap_A4( long a4 : __D0 ) : __D0
{
	EXG      D0,A4
}

class temp_A4
{
	private:
		long saved_A4;
		
		// non-copyable
		temp_A4           ( const temp_A4& );
		temp_A4& operator=( const temp_A4& );
	
	public:
		temp_A4() : saved_A4( swap_A4( module_A4 ) )
		{
		}
		
		~temp_A4()
		{
			swap_A4( saved_A4 );
		}
};

inline
asm void module_A4_suspend()
{
	MOVE.L   A4,module_A4
	JSR      0xFFFFFFF8
}

#endif
