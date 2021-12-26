/*
	string_common.hh
	----------------
*/

#ifndef PLUS_STRINGCOMMON_HH
#define PLUS_STRINGCOMMON_HH


namespace vxo
{
	
	struct datum_storage;
	
}

namespace plus
{
	
	void string_check_size( long size );
	
	void string_reserve( vxo::datum_storage& datum, long capacity );
	
	void string_set_length( vxo::datum_storage& datum, long length, char* data );
	
}

#endif
