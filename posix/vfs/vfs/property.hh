/*
	property.hh
	-----------
*/

#ifndef VFS_PROPERTY_HH
#define VFS_PROPERTY_HH

// plus
#include "plus/var_string_fwd.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	enum
	{
		no_fixed_size = -1,
	};
	
	typedef void (*property_get_hook)( plus::var_string&  out,
	                                   const node*        that,
	                                   bool               binary );
	
	typedef void (*property_set_hook)( const node*  that,
	                                   const char*  begin,
	                                   const char*  end,
	                                   bool         binary );
	
	struct readonly_property
	{
		static const int fixed_size = no_fixed_size;
		
		static const bool can_get = true;
		static const bool can_set = false;
		
		// Not defined
		static void set( const node* that, const char* begin, const char* end, bool binary );
	};
	
	struct writeonly_property
	{
		static const int fixed_size = no_fixed_size;
		
		static const bool can_get = false;
		static const bool can_set = true;
		
		// Not defined
		static void get( plus::var_string& result, const node* that, bool binary );
	};
	
	struct readwrite_property
	{
		static const int fixed_size = no_fixed_size;
		
		static const bool can_get = true;
		static const bool can_set = true;
	};
	
	struct property_params
	{
		int                size;
		property_get_hook  get;
		property_set_hook  set;
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
