/*
	symdesc.hh
	----------
*/

#ifndef VLIB_TYPES_SYMDESC_HH
#define VLIB_TYPES_SYMDESC_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class SymDesc : public Value
	{
		public:
			struct fields
			{
				short depth;
				short index;
			};
			
			static bool test( const Value& v )
			{
				return v.type() == V_desc;
			}
			
			SymDesc( int depth, int index ) : Value( V_desc )
			{
				const fields desc = { depth, index };
				
				pod_cast< fields >() = desc;
				
				set_cycle_free();
			}
			
			const fields& get() const  { return pod_cast< fields >(); }
			
			short depth() const  { return get().depth; }
			short index() const  { return get().index; }
	};
	
}

#endif
