/*
	refs.hh
	-------
*/

#ifndef VLIB_DISPATCH_REFS_HH
#define VLIB_DISPATCH_REFS_HH


namespace vlib
{
	
	class Value;
	
	typedef void (*put_ref)( const Value& v, void* param );
	
	typedef void (*get_refs)( const Value& v, put_ref put, void* param );
	
	struct refs
	{
		get_refs getrefs;
	};
	
}

#endif
