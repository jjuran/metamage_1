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

// Nitrogen core
#include "Nitrogen/Assert.h"

// Nitrogen / Carbon support
#include "Nitrogen/Aliases.h"
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
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
			return N::Convert< N::FSDirSpec >( result );
		}
		
		// Then root, or bust
		return N::RootDirectory( N::BootVolume() ) << "j";
	}
	
	static std::string ExpandVirtualUnixPathname( const char* path )
	{
		ASSERT( path != NULL );
		
		std::string pathname = path;
		
		if (    PathBeginsWith( pathname, "/bin"  )
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
		
		std::string scratch;
		const char *p, *q;
		
		p = q = pathname;
		
		if ( pathname[ 0 ] == '/' )
		{
			dir = N::RootDirectory( N::BootVolume() );
			q = ++p;  // skip the leading slash
			
			// advance to the next slash
			while ( ( *q != '\0' ) && ( *q != '/' ) )
			{
				++q;
			}
			
			if ( *q == '/' )
			{
				q++;  // skip the slash
				
				if ( std::string( p, q - p ) == "Volumes/" )
				{
					p = q;
					
					// advance to the third slash, if any
					while ( ( *q != '\0' ) && ( *q != '/' ) )
					{
						++q;
					}
					
					if ( p == q )
					{
						throw N::FNFErr();
					}
					
					std::string volName( p, q - p );
					dir = N::RootDirectory( DetermineVRefNum( volName ) );
					p = ++q;  // skip the slash
				}
			}
			
			q = p;  // backtrack, restore q
		}
		else if ( pathname[ 0 ] == '~' )
		{
			q = ++p;  // skip the tilde
			
			// advance to the next slash
			while ( ( *q != '\0' ) && ( *q != '/' ) )
			{
				++q;
			}
			
			if ( q == p )
			{
				// First path component is ~, home dir (system volume for now)
				dir = N::RootDirectory( N::BootVolume() );
			}
			else
			{
				// Otherwise, it's someone else's home dir.
				// For us, it's a named volume.  Copy the name.
				scratch = UntweakMacFilename( std::string( p, q - p ) ) + ":";
				dir = N::RootDirectory( DetermineVRefNum( scratch ) );
			}
			
			// skip the slash
			if ( *q != '\0' )
			{
				++q;
			}
			
			p = q;
		}
		
		// p and q now point to the first path component which is not special
		// (i.e. not "" or "~..."), or the end of the string.
		
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
						// parent dir
						dir = N::FSpGetParent( N::Convert< FSSpec >( dir ) );
						break;
					}
					// Otherwise, not . or ..
					/* fall thru */
				
				default:  // 
					scratch = UntweakMacFilename( dirName );
					FSSpec spec = dir & scratch;
					
					if ( *q == '/' )
					{
						// expecting a directory
						dir = N::Convert< N::FSDirSpec >( FSSpec( N::ResolveAliasFile( spec, true ) ) );
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
		
		return N::FSDirSpec( dir ) & scratch;
	}
	
	static std::string FSSpec2UnixAbsolutePathname( const FSSpec& item )
	{
		std::vector< std::string > rpath;
		
		FSSpec spec = item;
		
		while ( spec.parID != fsRtParID )
		{
			rpath.push_back( N::Convert< std::string >( spec.name ) );
			spec = N::Convert< FSSpec >( N::FSpGetParent( spec ) );
		}
		
		if ( spec.vRefNum != N::BootVolume().Get() )
		{
			rpath.push_back( N::Convert< std::string >( spec.name ) );
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

