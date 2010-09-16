/*
	serialize.hh
	------------
*/

#ifndef PLUS_SERIALIZE_HH
#define PLUS_SERIALIZE_HH

// Standard C/C++
#include <cstddef>

// plus
#include "plus/deconstruct.hh"
#include "plus/deconstruct_bool.hh"
#include "plus/freeze.hh"
#include "plus/reconstruct.hh"
#include "plus/stringify.hh"
#include "plus/thaw.hh"
#include "plus/vivify.hh"


namespace plus
{
	
	template < class POD >
	struct serialize_POD
	{
		typedef POD result_type;
		
		static const std::size_t fixed_size = sizeof (POD);
		
		typedef freeze_pod       freeze;
		typedef thaw_pod< POD >  thaw;
	};
	
	struct serialize_bool : serialize_POD< bool >
	{
		typedef stringify_bool  stringify;
		typedef vivify_bool     vivify;
		
		typedef deconstruct_bool                   deconstruct;
		typedef reconstruct< bool, thaw, vivify >  reconstruct;
	};
	
	template < class Int >
	struct serialize_int : serialize_POD< Int >
	{
		typedef stringify_int      stringify;
		typedef vivify_int< Int >  vivify;
		
		typedef typename serialize_POD< Int >::freeze  freeze;
		typedef typename serialize_POD< Int >::thaw    thaw;
		
		typedef deconstruct< freeze, stringify >  deconstruct;
		typedef reconstruct< Int, thaw, vivify >  reconstruct;
	};
	
	template < class Int >
	struct serialize_unsigned : serialize_POD< Int >
	{
		typedef stringify_unsigned      stringify;
		typedef vivify_unsigned< Int >  vivify;
		
		typedef typename serialize_POD< Int >::freeze  freeze;
		typedef typename serialize_POD< Int >::thaw    thaw;
		
		typedef deconstruct< freeze, stringify >  deconstruct;
		typedef reconstruct< Int, thaw, vivify >  reconstruct;
	};
	
	template <>
	struct serialize_unsigned< unsigned long long > : serialize_POD< unsigned long long >
	{
		typedef stringify_unsigned_wide  stringify;
		
		typedef deconstruct< freeze, stringify >  deconstruct;
	};
	
	template < class Int >
	struct serialize_hex : serialize_POD< Int >
	{
		typedef typename hex_stringifier< sizeof (Int) >::type  stringify;
		
		typedef typename serialize_POD< Int >::freeze  freeze;
		
		typedef deconstruct< freeze, stringify >  deconstruct;
	};
	
	struct serialize_pointer : serialize_POD< const void* >
	{
		typedef stringify_pointer  stringify;
		
		typedef deconstruct< freeze, stringify >  deconstruct;
	};
	
	struct serialize_c_string_contents
	{
		typedef const char* result_type;
		
		static const std::size_t fixed_size = 0;
		
		typedef plus::stringify_c_string  stringify;
		typedef plus::stringify_c_string  freeze;  // Omit trailing NUL byte
		
		typedef deconstruct< freeze, stringify >  deconstruct;
	};
	
	struct serialize_pascal_string_contents
	{
		static const std::size_t fixed_size = 0;
		
		typedef plus::stringify_pascal_string  stringify;
		typedef plus::stringify_pascal_string  freeze;  // Omit trailing NUL byte
		
		typedef deconstruct< freeze, stringify >  deconstruct;
	};
	
}

#endif

