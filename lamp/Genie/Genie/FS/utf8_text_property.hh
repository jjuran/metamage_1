/*
	utf8_text_property.hh
	---------------------
*/

#ifndef GENIE_FS_UTF8TEXTPROPERTY_HH
#define GENIE_FS_UTF8TEXTPROPERTY_HH

// plus
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	template < class Mac_Text_Property >
	struct utf8_text_property
	{
		static const int fixed_size = -1;
		
		static const bool can_get = Mac_Text_Property::can_get;
		static const bool can_set = Mac_Text_Property::can_set;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			Mac_Text_Property::get( result, that, binary );
			
			result = plus::utf8_from_mac( result );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			plus::string mac_text = plus::mac_from_utf8( begin, end - begin );
			
			begin = mac_text.data();
			end   = mac_text.size() + begin;
			
			Mac_Text_Property::set( that, begin, end, binary );
		}
	};
	
}

#endif

