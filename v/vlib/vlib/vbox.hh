/*
	vbox.hh
	-------
*/

#ifndef VLIB_VBOX_HH
#define VLIB_VBOX_HH

// plus
#include "plus/datum_storage.hh"
#include "plus/ibox.hh"


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
	
	typedef plus::datum_allocation  vu_string;
	typedef plus::ibox_structure    vu_ibox;
	
	struct vu_alloc : vu_string
	{
		char  type;       // vbox_type
		char  semantics;  // value_type
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
			bool has_extent() const  { return type() < 0; }
			
			void destroy_extent();
			
			void destroy()
			{
				if ( has_extent() )
				{
					destroy_extent();
				}
			}
			
			void unshare();
		
		public:
			~vbox()  { destroy(); }
			
			vbox( char semantics = 0 )
			{
				u.alloc.type      = Vbox_empty;
				u.alloc.semantics = semantics;
			}
			
			vbox( const vu_ibox& ix, char semantics );
			
			vbox( const vu_string& s, char semantics );
			
			vbox( const void* p, char semantics )
			{
				u.alloc.pointer   = (char*) p;
				u.alloc.type      = Vbox_pointer;
				u.alloc.semantics = semantics;
			}
			
			vbox( unsigned long n, destructor dtor, char semantics );
			
			vbox( const vbox& that );
			
			vbox& operator=( const vbox& that );
			
			void swap( vbox& b );
			
			const void* transfer_extent();
			
			unsigned long refcount() const;
			
			vbox_type type() const  { return vbox_type( u.alloc.type ); }
			
			char semantics() const  { return u.alloc.semantics; }
			
			const void* pointer() const  { return u.alloc.pointer; }
			
			friend unsigned long area( const vbox& box );
	};
	
	inline void swap( vbox& a, vbox& b )
	{
		a.swap( b );
	}
	
	unsigned long area( const vbox& box );
	
}

#endif
