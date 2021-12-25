/*
	vbox.hh
	-------
*/

#ifndef VLIB_VBOX_HH
#define VLIB_VBOX_HH

// Standard C
#include <stdint.h>

// vxo
#include "vxo/datum_storage.hh"

// bignum
#include "bignum/ibox.hh"


#if defined( __MWERKS__ )  &&  defined( __POWERPC__ )
/*
	This works around a compiler bug.  MWCPPC 2.4.1, with optimizations on,
	is testing the vbox's `type` byte (to determine if it later needs to call
	destroy_extent()) *before* constructing the vbox, when storing the result
	of a function returning a Value onto the stack as a const Value.
	
	We're not limiting the workaround based on CONFIG_DEBUGGING, because I'm
	not certain exactly which compiler options trigger the failure mode, and
	we can't rule out certain optimizations being switched on for debug builds
	(which are already using -O2 with Metrowerks C/C++).  If a new build tool
	is developed that mistakenly reverses CONFIG_DEBUGGING or specifies the
	wrong optimization level, the consequences shouldn't include a broken V
	interpreter -- especially since the next build tool will in all likelihood
	be written in V.  Anyway, an examination of the object code generated for
	a debug build of vc's main.cc reveals that it makes no difference.
	
	We also have to do this for the `semantics` byte, for the same reason.
	There, the symptom is spurious "arguments don't match function prototype"
	exceptions.
*/

	typedef volatile char Char;

#else

	typedef char Char;

#endif

namespace vlib
{
	
	typedef void (*destructor)( void* );
	
	enum vbox_type
	{
		Vbox_alloc = -1,
		Vbox_empty,
		Vbox_bigint,
		Vbox_string,
		Vbox_pointer,
	};
	
	typedef vxo::datum_allocation   vu_string;
	typedef bignum::ibox_structure  vu_ibox;
	
	struct vu_alloc : vu_string
	{
		// Char is a possibly volatile char.  See above for details.
		
		Char  type;       // vbox_type
		Char  semantics;  // value_type
		short flags;
	};
	
	struct vu_chars
	{
		signed char data[ sizeof (vu_string) ];
	};
	
	union vu
	{
		vu_alloc   alloc;
		vu_string  string;
		vu_chars   chars;
		vu_ibox    ibox;
	};
	
	class vbox
	{
		private:
			vu u;
		
		private:
			void destroy_extent();
			
			void destroy()
			{
				if ( has_extent() )
				{
					destroy_extent();
				}
			}
		
		public:
			~vbox()  { destroy(); }
			
			vbox( char semantics = 0, short flags = 0 )
			{
				u.alloc.type      = Vbox_empty;
				u.alloc.semantics = semantics;
				u.alloc.flags     = flags;
			}
			
			vbox( const vu_ibox& ix, char semantics, short flags = 0 );
			
			vbox( const vu_string& s, char semantics, short flags = 0 );
			
			vbox( const void* p, char semantics, short flags = 0 )
			{
				u.alloc.pointer   = (char*) p;
				u.alloc.type      = Vbox_pointer;
				u.alloc.semantics = semantics;
				u.alloc.flags     = flags;
			}
			
			vbox( unsigned long n, destructor dtor, char semantics );
			
			vbox( const vbox& that );
			
			vbox& operator=( const vbox& that );
			
			void swap( vbox& b );
			
			const void* transfer_extent();
			
			unsigned long refcount() const;
			
			bool has_extent() const  { return int8_t( u.alloc.type ) < 0; }
			
			char semantics() const  { return u.alloc.semantics; }
			
			short flags() const  { return u.alloc.flags; }
			
			void set_flags( short flags )  { u.alloc.flags = flags; }
			
			const void* pointer() const  { return u.alloc.pointer; }
			
			void secret() const;
			
			void unshare();
			
			friend unsigned long area( const vbox& box );
	};
	
	inline void swap( vbox& a, vbox& b )
	{
		a.swap( b );
	}
	
	unsigned long area( const vbox& box );
	
}

#endif
