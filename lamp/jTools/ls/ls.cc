/*	=====
 *	ls.cc
 *	=====
 */

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;

using std::string;


static void ListItem( const FSSpec& item )
{
	Io::Out << item.name << "\n";
}

static void IterateDir( const N::FSDirSpec& dir )
{
	std::for_each( N::FSContents( dir ).begin(),
	               N::FSContents( dir ).end(),
	               N::PtrFun( ListItem ) );
}

int O::Main(int argc, const char *const argv[])
{
	if ( argc > 1 )
	{
		int fail = 0;
		
		for ( int i = 1; i < argc; i++ )
		{
			FSSpec item;
			
			try
			{
				item = Path2FSS( argv[ i ] );
			}
			catch ( N::OSStatus err )
			{
				Io::Err << argv[ 0 ]
				        << ": "
				        << argv[ i ]
				        << ": No such file or directory"
				        << err
				        << "\n";
				fail++;
				continue;
			}
			
			if ( !N::FSpTestDirectoryExists( item ) )
			{
				ListItem( item );
			}
			else
			{
				try
				{
					IterateDir( NN::Convert< N::FSDirSpec >( item ) );
				}
				catch ( ... )
				{
				 	fail++;
				}
			}
		}
		return fail;
	}
	else
	{
		try
		{
			IterateDir( NN::Convert< N::FSDirSpec >( CurrentDirectory() ) );
		}
		catch ( ... )
		{
			return 1;  // Generic error
		}
	}
	
	return 0;
}

