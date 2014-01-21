/*
	unit_table.hh
	-------------
*/

#ifndef MACSYS_UNITTABLE_HH
#define MACSYS_UNITTABLE_HH


struct AuxDCE;


namespace mac   {
namespace types {
	
	struct AuxDCE;
	
}
}

namespace mac {
namespace sys {
	
	mac::types::AuxDCE*** get_unit_table_base();
	
	short get_unit_table_entry_count();
	
	const unsigned char* get_driver_name( mac::types::AuxDCE** dce );
	
}
}

#endif

