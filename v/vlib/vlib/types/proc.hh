/*
	proc.hh
	-------
*/

#ifndef VLIB_TYPES_PROC_HH
#define VLIB_TYPES_PROC_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Proc : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == V_proc;
			}
			
			Proc( const proc_info& proc ) : Value( proc )
			{
			}
	};
	
}

#endif
