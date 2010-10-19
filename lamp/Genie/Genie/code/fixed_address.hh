/*
	fixed_address.hh
	----------------
*/

#ifndef GENIE_CODE_FIXEDADDRESS_HH
#define GENIE_CODE_FIXEDADDRESS_HH

// Genie
#include "Genie/code/exec_handle.hh"


namespace Genie
{
	
	class fixed_address : public exec_handle
	{
		private:
			lamp_entry its_entry_point;
		
		public:
			fixed_address( lamp_entry entry ) : its_entry_point( entry )
			{
			}
			
			lamp_entry get_main_entry_point() const  { return its_entry_point; }
	};
	
}

#endif

