// mode_t.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_MODE_T_HH
#define POSEVEN_TYPES_MODE_T_HH

// POSIX
#include <sys/stat.h>

// Nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif


namespace poseven
{
	
	enum mode_t
	{
		s_ifmt  = S_IFMT,
		s_ifblk = S_IFBLK,
		s_ifchr = S_IFCHR,
		s_ififo = S_IFIFO,
		s_ifdir = S_IFDIR,
		s_iflnk = S_IFLNK,
		
		s_irwxu = S_IRWXU,
		s_irusr = S_IRUSR,
		s_iwusr = S_IWUSR,
		s_ixusr = S_IXUSR,
		s_irwxg = S_IRWXG,
		s_irgrp = S_IRGRP,
		s_iwgrp = S_IWGRP,
		s_ixgrp = S_IXGRP,
		s_irwxo = S_IRWXO,
		s_iroth = S_IROTH,
		s_iwoth = S_IWOTH,
		s_ixoth = S_IXOTH,
		s_isuid = S_ISUID,
		s_isgid = S_ISGID,
		
	#ifdef S_ISVTX
		
		s_isvtx = S_ISVTX,
		
	#endif
		
		_400 = 0400,
		_444 = 0444,
		_600 = 0600,
		_666 = 0666,
		_700 = 0700,
		_777 = 0777,
		
		mode_t_max = nucleus::enumeration_traits< ::mode_t >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( mode_t )
	
	inline bool s_isblk ( mode_t mode )  { return S_ISBLK ( mode ); }
	inline bool s_ischr ( mode_t mode )  { return S_ISCHR ( mode ); }
	inline bool s_isdir ( mode_t mode )  { return S_ISDIR ( mode ); }
	inline bool s_isfifo( mode_t mode )  { return S_ISFIFO( mode ); }
	inline bool s_isreg ( mode_t mode )  { return S_ISREG ( mode ); }
	inline bool s_islnk ( mode_t mode )  { return S_ISLNK ( mode ); }
	
	inline bool s_isblk ( const struct stat& sb )  { return S_ISBLK ( sb.st_mode ); }
	inline bool s_ischr ( const struct stat& sb )  { return S_ISCHR ( sb.st_mode ); }
	inline bool s_isdir ( const struct stat& sb )  { return S_ISDIR ( sb.st_mode ); }
	inline bool s_isfifo( const struct stat& sb )  { return S_ISFIFO( sb.st_mode ); }
	inline bool s_isreg ( const struct stat& sb )  { return S_ISREG ( sb.st_mode ); }
	inline bool s_islnk ( const struct stat& sb )  { return S_ISLNK ( sb.st_mode ); }
	
}

#endif
