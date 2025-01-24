/*
	float.hh
	--------
*/

#ifndef VLIB_TYPES_FLOAT_HH
#define VLIB_TYPES_FLOAT_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	static inline
	bool has_floating_point_math()
	{
	#ifdef __MACOS__
	#ifdef __MC68K__
		
		#define LOWMEM_ADDR( addr, type )  ((type*) (addr))
		
		#define AppPacks  LOWMEM_ADDR( 0x0AB8, char** )
		
		/*
			Return true if 'PACK' 4 has been loaded.
		*/
		
		return (int) AppPacks[ 4 ];
		
		#undef AppPacks
		#undef LOWMEM_ADDR
		
	#endif
	#endif
		
		return true;
	}
	
	typedef double float_type;
	
	class Integer;
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch float_dispatch;
	
	class Float : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &float_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			Float( float_type f );
			
			float_type const& get() const  { return pod_cast< float_type >(); }
			float_type      & get()        { return pod_cast< float_type >(); }
			
			operator float_type() const  { return get(); }
	};
	
	extern const type_info float_vtype;
	
}

#endif
