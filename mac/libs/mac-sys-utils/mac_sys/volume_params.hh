/*
	volume_params.hh
	----------------
*/

#ifndef MACSYS_VOLUMEPARAMS_HH
#define MACSYS_VOLUMEPARAMS_HH

// Mac OS
//#include <Files.h>
struct GetVolParmsInfoBuffer;

typedef short OSErr;


namespace mac {
namespace sys {
	
	OSErr get_volume_params( GetVolParmsInfoBuffer& buffer, short vRefNum );
	
	bool volume_is_on_server( short vRefNum );
	
	template < class Item >
	inline bool item_is_on_server( const Item& item )
	{
		return volume_is_on_server( item.vRefNum );
	}
	
}
}

#endif
