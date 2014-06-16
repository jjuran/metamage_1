/*
	fifo.hh
	-------
*/

#ifndef RELIX_FS_FIFO_HH
#define RELIX_FS_FIFO_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace relix
{
	
	enum fifo_state
	{
		FIFO_closed            = 'O' - 'a',  // -18
		
		FIFO_half_open_reading = '<' - 'a',  // -37
		FIFO_half_open_writing = '>' - 'a',  // -35
		
		FIFO_empty             = 'a' - 'a',  //   0
		FIFO_1_of_17           = 'b' - 'a',  //   1
		FIFO_2_of_17           = 'c' - 'a',  //   2
		FIFO_3_of_17           = 'd' - 'a',  //   3
		FIFO_4_of_17           = 'e' - 'a',  //   4
		FIFO_5_of_17           = 'f' - 'a',  //   5
		FIFO_6_of_17           = 'g' - 'a',  //   6
		FIFO_7_of_17           = 'h' - 'a',  //   7
		FIFO_8_of_17           = 'i' - 'a',  //   8
		FIFO_9_of_17           = 'j' - 'a',  //   9
		FIFO_10_of_17          = 'k' - 'a',  //  10
		FIFO_11_of_17          = 'l' - 'a',  //  11
		FIFO_12_of_17          = 'm' - 'a',  //  12
		FIFO_13_of_17          = 'n' - 'a',  //  13
		FIFO_14_of_17          = 'o' - 'a',  //  14
		FIFO_15_of_17          = 'p' - 'a',  //  15
		FIFO_16_of_17          = 'q' - 'a',  //  16
		FIFO_full              = 'r' - 'a',  //  17
		
		FIFO_underrun          = '_' - 'a',  //  -2
		FIFO_overrun           = '!' - 'a',  // -64
		
		FIFO_EOF               = '.' - 'a',  // -51
		FIFO_broken_pipe       = '"' - 'a',  // -63
	};
	
	inline fifo_state fifo_state_for_ratio( int numer, int denom )
	{
		if ( denom == FIFO_full )
		{
			return fifo_state( numer );
		}
		
		return fifo_state( 1.0 * numer / denom * FIFO_full );
	}
	
	typedef void (*fifo_update_f)( vfs::filehandle& that, fifo_state state );
	
	vfs::filehandle_ptr open_fifo( const vfs::node*  that,
	                               int               flags,
	                               fifo_update_f     update = NULL );
	
}

#endif
