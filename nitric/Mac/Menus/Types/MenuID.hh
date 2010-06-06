/*
	Mac/Menus/Types/MenuID.hh
	-------------------------
*/

#ifndef MAC_MENUS_TYPES_MENUID_HH
#define MAC_MENUS_TYPES_MENUID_HH

#ifndef __MENUS__
#include <Menus.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/disposer.hh"


namespace Mac
{
	
	enum MenuID
	{
		kMenuID_Max = nucleus::enumeration_traits< ::MenuID >::max
	};
	
}

namespace nucleus
{
	
	template <> struct disposer< Mac::MenuID >
	{
		typedef Mac::MenuID  argument_type;
		typedef void         result_type;
		
		void operator()( Mac::MenuID id ) const
		{
			::DeleteMenu( id );
		}
	};
	
}

#endif

