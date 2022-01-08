/*
	symbol.hh
	---------
*/

#ifndef VXO_SYMBOL_HH
#define VXO_SYMBOL_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	class Symbol : public Box
	{
		public:
			static bool test( const Box& box )
			{
				return box.control_byte() == Box_pointer  &&
				       box.subtype_byte() == Box_symbolic;
			}
			
			Symbol( const char* name );
			
			const char* name() const  { return u.str.pointer; }
	};
	
	extern const char null[];
	
}

#endif
