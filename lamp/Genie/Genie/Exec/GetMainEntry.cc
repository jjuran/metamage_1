/*	===============
 *	GetMainEntry.cc
 *	===============
 */

#include "Genie/Exec/GetMainEntry.hh"

// Standard C++
#include <algorithm>

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/CodeFragments.hh"

// Genie
#include "Genie/Exec/MainEntryPoint.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	typedef int (*Extended_Entry)( int argc, iota::argv_t argv, iota::envp_t envp, Dispatcher dispatcher );
	
	
	class AddressMain : public MainEntryPoint
	{
		private:
			Standard_Entry itsEntry;
		
		public:
			AddressMain( Standard_Entry entry ) : itsEntry( entry )
			{
				ASSERT( itsEntry != NULL );
			}
			
			AddressMain( Trivial_Entry entry ) : itsEntry( (Standard_Entry) entry )
			{
				ASSERT( itsEntry != NULL );
			}
			
			int Invoke( int argc, iota::argv_t argv, iota::envp_t envp, Dispatcher dispatcher )
			{
				ASSERT( itsEntry != NULL );
				
				Extended_Entry entry = (Extended_Entry) itsEntry;
				
				return entry( argc, argv, envp, dispatcher );
			}
	};
	
	
	class BinaryImageClient
	{
		private:
			BinaryImage itsBinaryImage;
		
		public:
			BinaryImageClient( const BinaryImage& image ) : itsBinaryImage( image )
			{
			}
	};
	
	class CodeResourceMain : public BinaryImageClient, public AddressMain
	{
		public:
			CodeResourceMain( const BinaryImage& image ) : BinaryImageClient( image ),
			                                               AddressMain( reinterpret_cast< Standard_Entry >( *image.get().Get() ) )
			{
			}
	};
	
	
	static inline n::owned< N::CFragConnectionID > ConnectToFragment( const BinaryImage& image )
	{
		// Set up dispatcher and envp
		
		return N::GetMemFragment< N::kPrivateCFragCopy >( *image.get(),
		                                                  N::GetHandleSize( image ) );
	}
	
	
	static void LoadSymbol( N::CFragConnectionID fragment, ConstStr255Param symName, void* value )
	{
		void** symbol = NULL;
		
		N::FindSymbol( fragment, symName, &symbol );
		
		*symbol = value;
	}
	
	class CFMPluginMain : public MainEntryPoint
	{
		private:
			BinaryImage                       itsBinaryImage;
			n::owned< N::CFragConnectionID >  itsFragmentConnection;
		
		public:
			CFMPluginMain( const BinaryImage& image ) : itsBinaryImage( image )
			{
			}
			
			int Invoke( int argc, iota::argv_t argv, iota::envp_t envp, Dispatcher dispatcher );
	};
	
	int CFMPluginMain::Invoke( int argc, iota::argv_t argv, iota::envp_t envp, Dispatcher dispatcher )
	{
		Extended_Entry lamp_main = NULL;
		
		if ( itsFragmentConnection.get() == N::CFragConnectionID() )
		{
			itsFragmentConnection = ConnectToFragment( itsBinaryImage );
			
			N::FindSymbol( itsFragmentConnection, "\p" "_lamp_main", &lamp_main );
		}
		
		ASSERT( lamp_main != NULL );
		
		return lamp_main( argc, argv, envp, dispatcher );
	}
	
	
#if TARGET_RT_MAC_CFM
	
	typedef CFMPluginMain ExternalBinaryMain;
	
#else
	
	typedef CodeResourceMain ExternalBinaryMain;
	
#endif
	
	static inline MainEntry seize_ptr( MainEntryPoint* entry )
	{
		return MainEntry( entry );
	}
	
	MainEntry GetMainEntryFromAddress( Trivial_Entry address )
	{
		return seize_ptr( new AddressMain( address ) );
	}
	
	MainEntry GetMainEntryFromAddress( Standard_Entry address )
	{
		return seize_ptr( new AddressMain( address ) );
	}
	
	MainEntry GetMainEntryFromBinaryImage( const BinaryImage& binary )
	{
		return seize_ptr( new ExternalBinaryMain( binary ) );
	}
	
}

