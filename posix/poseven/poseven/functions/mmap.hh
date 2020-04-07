/*
	mmap.hh
	-------
*/

#ifndef POSEVEN_FUNCTIONS_MMAP_HH
#define POSEVEN_FUNCTIONS_MMAP_HH

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif
#ifndef POSEVEN_TYPES_MMAP_T_HH
#include "poseven/types/mmap_t.hh"
#endif


namespace poseven
{
	
	enum prot_t
	{
		prot_none  = PROT_NONE,
		prot_read  = PROT_READ,
		prot_write = PROT_WRITE,
		prot_exec  = PROT_EXEC,
		
		prot_t_max = nucleus::enumeration_traits< int >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( prot_t )
	
	enum mmap_flags_t
	{
		map_shared  = MAP_SHARED,
		map_private = MAP_PRIVATE,
		
	#ifdef MAP_COPY
		
		map_copy    = MAP_COPY,
		
	#endif
		
		map_fixed        = MAP_FIXED,
		
	#ifdef MAP_RENAME
		
		map_rename       = MAP_RENAME,
		
	#endif
		
	#ifdef MAP_NORESERVE
		
		map_noreserve    = MAP_NORESERVE,
		
	#endif
		
	#ifdef MAP_INHERIT
		
		map_inherit      = MAP_INHERIT,
		
	#endif
		
	#ifdef MAP_NOEXTEND
		
		map_noextend     = MAP_NOEXTEND,
		
	#endif
		
	#ifdef MAP_HASSEMAPHORE
		
		map_hassemaphore = MAP_HASSEMAPHORE,
		
	#endif
		
	#ifdef MAP_TRYFIXED
		
		map_tryfixed     = MAP_TRYFIXED,
		
	#endif
		
	#ifdef MAP_FILE
		
		map_file      = MAP_FILE,
		
	#endif
		
	#ifdef MAP_ANON
		
		map_anon      = MAP_ANON,
		
	#endif
		
	#ifdef MAP_ANONYMOUS
		
		map_anonymous = MAP_ANONYMOUS,
		
	#endif
		
	#ifdef MAP_FLAGMASK
		
		map_flagmask  = MAP_FLAGMASK,
		
	#endif
		
		mmap_flags_t_max = nucleus::enumeration_traits< int >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( mmap_flags_t )
	
	
	nucleus::owned< mmap_t > mmap( size_t        len,
	                               prot_t        prot,
	                               mmap_flags_t  flags,
	                               fd_t          fd,
	                               off_t         offset = 0 );
	
}

#endif
