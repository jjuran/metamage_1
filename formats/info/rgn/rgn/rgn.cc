/*
	rgn.cc
	------
*/

#include "rgn/rgn.hh"


namespace rgn
{

bool valid( const header& h, uint32_t size )
{
	if ( size < sizeof h )
	{
		return false;
	}
	
	/*
		It's not out of the question to support
		regions whose size is in 32K -> 64K in
		the future, but 32766 is the max for now.
		
		TODO:  Add more region integrity checks.
	*/
	
	const int16_t data_size = h.size;
	
	return data_size >= 10         &&
	       (data_size & 0x1) == 0  &&
	       size >= data_size;
}

}
