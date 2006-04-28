/*	============
 *	pathnames.cc
 *	============
 */

#include "Genie/pathnames.hh"

// Universal Interfaces
#include <MacErrors.h>

// Standard C/C++
#include <cctype>

// Standard C++
#include <vector>

// MoreFiles
#include "MoreFilesExtras.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen / Carbon support
#include "Nitrogen/Aliases.h"
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static bool PathBeginsWith( const std::string& str, const std::string& sub )
	{
		// str must begin with sub and the following char must be '/',
		// unless str == sub.
		
		return    (    str.size() == sub.size()
		            || str.size() >  sub.size() && str[ sub.size() ] == '/' )
		       && std::equal( sub.begin(),
		                      sub.end(),
		                      str.begin() );
	}
	
	static N::FSDirSpec FindJDirectory()
	{
		FSSpec result;
		
		// Try current directory first
		if ( N::FSpTestDirectoryExists( result = N::FSDirSpec() & "j" ) )
		{
			return NN::Convert< N::FSDirSpec >( result );
		}
		
		// Then root, or bust
		return N::RootDirectory( N::BootVolume() ) << "j";
	}
	
	static std::string ExpandVirtualUnixPathname( const char* path )
	{
		ASSERT( path != NULL );
		
		std::string pathname = path;
		
		if (    PathBeginsWith( pathname, "/Developer" )
		     || PathBeginsWith( pathname, "/bin"  )
		     || PathBeginsWith( pathname, "/etc"  )
		     || PathBeginsWith( pathname, "/lib"  )
		     || PathBeginsWith( pathname, "/sbin" )
		     || PathBeginsWith( pathname, "/tmp"  )
		     || PathBeginsWith( pathname, "/usr"  )
		     || PathBeginsWith( pathname, "/var"  ) )
		{
			std::string j = N::FSDirGetPOSIXPathname( FindJDirectory() );
			j.resize( j.size() - 1 );  // Lose trailing slash
			pathname = j + pathname;
		}
		
		return pathname;
	}
	
	static std::string UntweakMacFilename( std::string name )
	{
		std::replace( name.begin(), name.end(), ':', '/' );
		
		return name;
	}
	
	static N::FSVolumeRefNum DetermineVRefNum( ConstStr255Param   name,
	                                           N::FSVolumeRefNum  vRefNum = N::FSVolumeRefNum() )
	{
		::FSVolumeRefNum actualVRefNum;
		N::ThrowOSStatus( ::DetermineVRefNum( name, vRefNum, &actualVRefNum ) );
		
		return N::FSVolumeRefNum( actualVRefNum );
	}
	
	static N::FSVolumeRefNum DetermineVRefNum( const std::string&  name,
	                                           N::FSVolumeRefNum   vRefNum = N::FSVolumeRefNum() )
	{
		return DetermineVRefNum( N::Str27( name ), vRefNum );
	}
	
	static N::FSDirSpec HomeDir( const std::string& user = std::string() )
	{
		return N::FSDirSpec();
	}
	
	static N::FSDirSpec VolumesDir()
	{
		FSSpec volumes = N::RootDirectory( N::BootVolume() ) & "Volumes";
		
		try
		{
			return NN::Convert< N::FSDirSpec >( volumes );
		}
		catch ( const N::FNFErr& err )
		{
			// continue
		}
		
		return N::FSpDirCreate( volumes );
	}
	
	static N::FSDirSpec DirLookupDir( const N::FSDirSpec& dir, const std::string& name )
	{
		if ( name == ""   )  return dir;
		if ( name == "."  )  return dir;
		
		if ( name == ".." )
		{
			if ( long( dir.dirID ) == long( fsRtDirID ) )
			{
				if ( dir.vRefNum == N::BootVolume() )
				{
					return dir;
				}
				
				return VolumesDir();
			}
			
			return N::FSpGetParent( NN::Convert< FSSpec >( dir ) );
		}
		
		std::string macName = UntweakMacFilename( name );
		
		if ( dir == VolumesDir() )
		{
			return N::RootDirectory( DetermineVRefNum( macName + ":" ) );
		}
		
		return dir << macName;
	}
	
	static FSSpec DirLookupSpec( const N::FSDirSpec& dir, const std::string& name )
	{
		if ( name == ""   )  return dir & "";
		if ( name == "."  )  return dir & "";
		
		if ( name == ".." )
		{
			if ( long( dir.dirID ) == long( fsRtDirID ) )
			{
				if ( dir.vRefNum == N::BootVolume() )
				{
					return dir & "";
				}
				
				return VolumesDir() & "";
			}
			
			return N::FSpGetParent( NN::Convert< FSSpec >( dir ) ) & "";
		}
		
		std::string macName = UntweakMacFilename( name );
		
		if ( dir == VolumesDir() )
		{
			return N::RootDirectory( DetermineVRefNum( macName + ":" ) ) & "";
		}
		
		return dir & macName;
	}
	
	static FSSpec UnixPathname2FSSpec( const N::FSDirSpec&  cwd,
	                                   const char*          pathname )
	{
		std::string realPath = ExpandVirtualUnixPathname( pathname );
		pathname = realPath.c_str();
		
		if ( pathname[ 0 ] == '\0' )
		{
			throw N::ParamErr();
		}
		
		N::FSDirSpec dir = cwd;
		
		const char *p, *q;
		
		p = q = pathname;
		
		if ( pathname[ 0 ] == '/' )
		{
			dir = N::RootDirectory( N::BootVolume() );
			q = ++p;  // skip the leading slash
		}
		else if ( pathname[ 0 ] == '~' )
		{
			q = ++p;  // skip the tilde
			
			// advance to the next slash
			while ( ( *q != '\0' ) && ( *q != '/' ) )
			{
				++q;
			}
			
			std::string username( p, q - p );
			
			dir = HomeDir( username );
			
			// skip the slash
			if ( *q != '\0' )
			{
				++q;
			}
			
			p = q;
		}
		
		// p and q now point to the first path component which is not special
		// (i.e. not "" or "~..."), or the end of the string.
		
		std::string scratch;
		
		while ( *q != '\0' )
		{
			// scan to the first/next slash
			while ( ( *q != '\0' ) && ( *q != '/' ) )
			{
				q++;
			}
			
			// copy the first/next item name
			std::string dirName( p, q - p );
			
			// ignore null path components
			if ( dirName.size() == 0 )
			{
				continue;
			}
			
			// check the first char
			switch ( p[ 0 ] )
			{
				case '.':  // current dir, parent dir, or dotfile
					if ( dirName.size() == 1 )
					{
						// current dir; no change
						break;
					}
					else if ( dirName.size() == 2  &&  p[ 1 ] == '.' )
					{
						dir = DirLookupDir( dir, dirName );
						break;
					}
					// Otherwise, not . or ..
					/* fall thru */
				
				default:  // 
					scratch = UntweakMacFilename( dirName );
					FSSpec spec = DirLookupSpec( dir, UntweakMacFilename( dirName ) );
					
					if ( *q == '/' )
					{
						// expecting a directory
						dir = NN::Convert< N::FSDirSpec >( FSSpec( N::ResolveAliasFile( spec, true ) ) );
						scratch = "";
					}
					else
					{
						// we're done
					}
					break;
			}  // switch
			
			// skip the slash
			if ( *q == '/' )
			{
				p = ++q;
			}
		}
		
		return DirLookupSpec( dir, scratch );
	}
	
	static std::string FSSpec2UnixAbsolutePathname( const FSSpec& item )
	{
		std::vector< std::string > rpath;
		
		FSSpec spec = item;
		
		while ( spec.parID != fsRtParID )
		{
			rpath.push_back( NN::Convert< std::string >( spec.name ) );
			spec = NN::Convert< FSSpec >( N::FSpGetParent( spec ) );
		}
		
		if ( spec.vRefNum != N::BootVolume().Get() )
		{
			rpath.push_back( NN::Convert< std::string >( spec.name ) );
			rpath.push_back( "Volumes" );
		}
		
		std::string pathname;
		std::vector< std::string >::reverse_iterator it, end = rpath.rend();
		
		for ( it = rpath.rbegin();  it != end;  ++it )
		{
			std::string name = *it;
			std::replace( name.begin(), name.end(), '/', ':' );
			pathname += "/" + name;
		}
		
		return pathname;
	}
	
	FSSpec ResolveUnixPathname( const char*          path,
	                            const N::FSDirSpec&  cwd )
	{
		return UnixPathname2FSSpec( cwd, path );
	}
	
}

