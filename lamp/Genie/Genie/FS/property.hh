/*
	property.hh
	-----------
*/

#ifndef GENIE_FS_PROPERTY_HH
#define GENIE_FS_PROPERTY_HH

// Standard C/C++
#include <cstddef>


namespace plus
{
	
	class var_string;	
	
}

namespace Genie
{
	
	class FSTree;
	
	typedef void (*property_get_hook)( plus::var_string&  out,
	                                   const FSTree*      that,
	                                   bool               binary );
	
	typedef void (*property_set_hook)( const FSTree  *that,
	                                   const char    *begin,
	                                   const char    *end,
	                                   bool           binary );
	
	struct readonly_property
	{
		static const std::size_t fixed_size = 0;
		
		static const bool can_get = true;
		static const bool can_set = false;
		
		// Not defined
		static void set( const FSTree* that, const char* begin, const char* end, bool binary );
	};
	
	struct writeonly_property
	{
		static const std::size_t fixed_size = 0;
		
		static const bool can_get = false;
		static const bool can_set = true;
		
		// Not defined
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
	struct readwrite_property
	{
		static const std::size_t fixed_size = 0;
		
		static const bool can_get = true;
		static const bool can_set = true;
	};
	
	struct property_params
	{
		const std::size_t        size;
		const property_get_hook  get;
		const property_set_hook  set;
	};
	
	template < class Property >
	struct property_params_factory
	{
		static const property_params value;
	};
	
	template < class Property >
	const property_params property_params_factory< Property >::value =
	{
		Property::fixed_size,
		Property::can_get ? &Property::get : 0,  // NULL
		Property::can_set ? &Property::set : 0   // NULL
	};
	
}

#endif

