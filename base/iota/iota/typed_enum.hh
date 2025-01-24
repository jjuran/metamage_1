/*
	typed_enum.hh
	-------------
*/

#ifndef IOTA_TYPEDENUM_HH
#define IOTA_TYPEDENUM_HH


namespace iota
{
	
	/*
		Example:
		
			TYPED_ENUM( Foobar, unsigned char )
			{
				Foo,
				Bar,
				Baz,
			};
		
		In most cases, the enumeration will be named and provide
		type safety benefits -- for example, `Foobar x = true;`
		would fail to compile.  When building with Metrowerks dev
		tools (for targeting classic Mac OS) however, make Foobar
		a simple type alias of unsigned char for better codegen
		(avoiding pointless sign extensions for comparisons).
		
		That means we lose the type safety in that scenario, but
		we still have it elsewhere, so errors are still caught.
	*/
	
#ifdef __MWERKS__
	
	#define TYPED_ENUM( name, type ) typedef type name; enum
	
#else
	
	#define TYPED_ENUM( name, type ) enum name
	
#endif
	
}

#endif
