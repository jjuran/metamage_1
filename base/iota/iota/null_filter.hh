/*
	iota/null_filter.hh
	-------------------
*/

#ifndef IOTA_NULLFILTER_HH
#define IOTA_NULLFILTER_HH


namespace iota
{
	
	template < class Constant, bool nonnull >
	struct null_filter_impl : Constant
	{
	};
	
	template < class Constant >
	struct null_filter_impl< Constant, false >
	{
		// no value
	};
	
	template < class Constant >
	struct null_filter : null_filter_impl< Constant, Constant::value >
	{
	};
	
}

#endif

