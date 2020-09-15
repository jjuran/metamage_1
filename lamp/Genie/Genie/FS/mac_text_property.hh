/*
	mac_text_property.hh
	--------------------
*/

#ifndef GENIE_FS_MACTEXTPROPERTY_HH
#define GENIE_FS_MACTEXTPROPERTY_HH

// plus
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	template < class UTF8_Text_Property >
	struct mac_text_property
	{
		static const int fixed_size = -1;
		
		static const bool can_get = UTF8_Text_Property::can_get;
		static const bool can_set = UTF8_Text_Property::can_set;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			UTF8_Text_Property::get( result, that, binary );
			
			result = plus::mac_from_utf8( result );
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			plus::string mac_text = plus::utf8_from_mac( begin, end - begin );
			
			begin = mac_text.data();
			end   = mac_text.size() + begin;
			
			UTF8_Text_Property::set( that, begin, end, binary );
		}
	};
	
}

#endif
