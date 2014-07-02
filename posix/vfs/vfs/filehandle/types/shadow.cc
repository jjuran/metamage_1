/*
	shadow.cc
	---------
*/

#include "vfs/filehandle/types/shadow.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/append.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/filehandle/primitives/pread.hh"
#include "vfs/filehandle/primitives/pwrite.hh"
#include "vfs/filehandle/primitives/seteof.hh"


namespace vfs
{
	
	struct shadow_extra
	{
		filehandle* basis;
	};
	
	class shadow_filehandle : public filehandle
	{
		public:
			shadow_filehandle( const node&  file,
			                   int          flags,
			                   filehandle&  basis );
	};
	
	static ssize_t shadow_pread( filehandle* that, char* buffer, size_t n, off_t offset )
	{
		shadow_extra& extra = *(shadow_extra*) that->extra();
		
		return pread( *extra.basis, buffer, n, offset );
	}
	
	static off_t shadow_geteof( filehandle* that )
	{
		shadow_extra& extra = *(shadow_extra*) that->extra();
		
		return geteof( *extra.basis );
	}
	
	static ssize_t shadow_pwrite( filehandle* that, const char* buffer, size_t n, off_t offset )
	{
		shadow_extra& extra = *(shadow_extra*) that->extra();
		
		return pwrite( *extra.basis, buffer, n, offset );
	}
	
	static void shadow_seteof( filehandle* that, off_t offset )
	{
		shadow_extra& extra = *(shadow_extra*) that->extra();
		
		seteof( *extra.basis, offset );
	}
	
	static ssize_t shadow_append( filehandle* that, const char* buffer, size_t n )
	{
		shadow_extra& extra = *(shadow_extra*) that->extra();
		
		return append( *extra.basis, buffer, n );
	}
	
	static const bstore_method_set shadow_bstore_methods =
	{
		&shadow_pread,
		&shadow_geteof,
		&shadow_pwrite,
		&shadow_seteof,
		&shadow_append,
	};
	
	static const filehandle_method_set shadow_methods =
	{
		&shadow_bstore_methods,
	};
	
	static void destroy_shadow( filehandle* that )
	{
		shadow_extra& extra = *(shadow_extra*) that->extra();
		
		intrusive_ptr_release( extra.basis );
	}
	
	filehandle_ptr new_shadow( const node& file, int flags, filehandle& basis )
	{
		filehandle* result = new filehandle( &file,
		                                     flags,
		                                     &shadow_methods,
		                                     sizeof (shadow_extra),
		                                     &destroy_shadow );
		
		shadow_extra& extra = *(shadow_extra*) result->extra();
		
		extra.basis = &basis;
		
		intrusive_ptr_add_ref( extra.basis );
		
		return result;
	}
	
}
