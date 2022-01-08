/*
	boolean.hh
	----------
*/

#ifndef VXO_BOOLEAN_HH
#define VXO_BOOLEAN_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	class Bool : public Box
	{
		public:
			static bool test( const Box& box )
			{
				return box.control_byte() == Box_boolean;
			}
			
			Bool( bool b )
			{
				u.chr[ 0 ] = b;
				
				set_subtype_byte( b );
				set_control_byte( Box_boolean );
			}
	};
	
}

#endif
