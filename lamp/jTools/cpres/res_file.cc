/*
	res_file.cc
	-----------
*/

// Standard C
#include <stdlib.h>
#include <string.h>

// Standard C++
#include <algorithm>

// plus
#include "plus/string.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// Divergence
#include "Divergence/Utilities.hh"

// cpres
#include "res_file.hh"


namespace n = nucleus;
namespace N = Nitrogen;
namespace Div = Divergence;


class ForkNames
{
	private:
		HFSUniStr255 name[ 2 ];
	
	public:
		ForkNames()
		{
			/*
				Notice that we check for FSOpenResourceFile() here, not
				FSGetResourceForkName().  In Mac OS 9.0 (or earlier with
				CarbonLib), the former is available and the latter is not.
				But we check for FSOpenResourceFile(), because without it,
				we don't ever use the resource fork name.
			*/
			
			if ( has_FSOpenResourceFile() )
			{
				if (OSErr err = ::FSGetResourceForkName( &name[ rsrcFork ] ) )
				{
					abort();
				}
			}
		}
		
		const HFSUniStr255* get() const  { return name; }
};

static ForkNames forkNames;

static const HFSUniStr255& getForkName( ForkType fork )
{
	return forkNames.get()[ fork ];
}

static inline const UniChar* get_string_data( const HFSUniStr255& str )
{
	return str.unicode;
}

static inline UniCharCount get_string_size( const HFSUniStr255& str )
{
	return str.length;
}

template < bool unicode >
struct file_manager_traits
{
	typedef FSSpec File;
	typedef FSSpec Node;
	
	static File resolve_path( const char* path )
	{
		return Div::ResolvePathToFSSpec( path );
	}
	
	static Node resolve_new_path( const char* path )
	{
		return Div::ResolvePathToFSSpec( path );
	}
};

template <>
struct file_manager_traits< true >
{
	typedef N::FSRefNameSpec FSRefNameSpec;
	
	typedef FSRef          File;
	typedef FSRefNameSpec  Node;
	
	static File resolve_path( const char* path )
	{
	#ifdef __RELIX__
		
		return N::FSpMakeFSRef( Div::ResolvePathToFSSpec( path ) );
		
	#endif
		
		return N::FSPathMakeRef( path );
	}
	
	static Node resolve_new_path( const char* path )
	{
		const char* slash = strrchr( path, '/' );
		
		const char* filename = slash ? slash + 1 : path;
		
		FSRef parent = resolve_path( slash ? plus::string( path, slash ).c_str() : "." );
		
		Node node;
		
		node.parent = parent;
		
		// FIXME:  This breaks if the filename isn't ASCII.
		const UniCharCount length = strlen( filename );
		
		node.name.length = length;
		
		std::copy( filename,
		           filename + length,
		           node.name.unicode );
		
		return node;
	}
};


static FSSpec create_res_file( const FSSpec& file, ForkType fork )
{
	::FSpCreateResFile( &file, 'RSED', 'rsrc', smRoman );
	
	return file;
}

static FSRef create_res_file( const Mac::FSRefNameSpec& file, ForkType fork )
{
	const HFSUniStr255& forkName = getForkName( fork );
	
	N::FSCreateResourceFile( file, forkName );
	
	return N::FSMakeFSRefUnicode( file, kTextEncodingUnknown );
}

static n::owned< N::ResFileRefNum > open_res_file( const FSSpec&   filespec,
                                                   ForkType        fork,
                                                   N::FSIOPermssn  perm )
{
	return N::FSpOpenResFile( filespec, perm );
}

static n::owned< N::ResFileRefNum > open_res_file( const FSRef&    file,
                                                   ForkType        fork,
                                                   N::FSIOPermssn  perm )
{
	const HFSUniStr255& forkName = getForkName( fork );
	
	return N::FSOpenResourceFile( file, forkName, perm );
}


template < bool unicode >
static n::owned< N::ResFileRefNum >
open_res_file_template( const char* path, ForkType fork, N::FSIOPermssn perm )
{
	typedef file_manager_traits< unicode > Traits;
	
	typename Traits::File destFile = Traits::resolve_path( path );
	
	return open_res_file( destFile, fork, perm );
}

template < bool unicode >
static n::owned< N::ResFileRefNum >
open_new_res_file_template( const char* path, ForkType fork )
{
	typedef file_manager_traits< unicode > Traits;
	
	typename Traits::Node destNode = Traits::resolve_new_path( path );
	
	typename Traits::File destFile = create_res_file( destNode, fork );
	
	return open_res_file( destFile, fork, N::fsRdWrPerm );
}

n::owned< N::ResFileRefNum >
open_res_file( const char* path, ForkType fork )
{
	enum { no_unicode, unicode };
	
	const N::FSIOPermssn ro = N::fsRdPerm;
	
	if ( has_FSOpenResourceFile() )
	{
		return open_res_file_template< unicode >( path, fork, ro );
	}
	
	return open_res_file_template< no_unicode >( path, fork, ro );
}

n::owned< N::ResFileRefNum >
open_res_file( const char* path, ForkType fork, bool exists )
{
	enum { no_unicode, unicode };
	
	const N::FSIOPermssn rw = N::fsRdWrPerm;
	
	if ( exists )
	{
		if ( has_FSOpenResourceFile() )
		{
			return open_res_file_template< unicode >( path, fork, rw );
		}
		
		return open_res_file_template< no_unicode >( path, fork, rw );
	}
	
	if ( has_FSOpenResourceFile() )
	{
		return open_new_res_file_template< unicode >( path, fork );
	}
	
	return open_new_res_file_template< no_unicode >( path, fork );
}

void set_BNDL_bit( const char* path, bool value )
{
	const FSSpec file = Div::ResolvePathToFSSpec( path );
	
	FInfo info = N::FSpGetFInfo( file );
	
	if ( value == ! (info.fdFlags & kHasBundle) )
	{
		info.fdFlags ^= kHasBundle;
		
		N::FSpSetFInfo( file, info );
	}
}
