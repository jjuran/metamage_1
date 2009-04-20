/*	============
 *	MainEntry.cc
 *	============
 */

#include "Genie/Exec/MainEntry.hh"

// Nitrogen
#include "Nitrogen/CodeFragments.h"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	MainEntryPoint::~MainEntryPoint()
	{
	}
	
	
	static void invoke_main( Main3 main3, int argc, iota::argv_t argv, iota::environ_t envp )
	{
		ASSERT( main3 != NULL );
		
		const int exit_status = main3( argc, argv, envp );
		
		CurrentProcess().Exit( exit_status );
	}
	
	class AddressMain : public MainEntryPoint
	{
		private:
			Main3 itsMain;
		
		public:
			AddressMain( Main3 main ) : itsMain( main )
			{
			}
			
			void Invoke( int argc, iota::argv_t argv, iota::environ_t envp )
			{
				invoke_main( itsMain, argc, argv, envp );
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
			                                               AddressMain( reinterpret_cast< Main3 >( *image.Get().Get() ) )
			{
			}
	};
	
	
	inline NN::Owned< N::CFragConnectionID > ConnectToFragment( const BinaryImage& image )
	{
		// Set up dispatcher and envp
		
		return N::GetMemFragment< N::kPrivateCFragCopy >( *image.Get(),
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
			BinaryImage                        itsBinaryImage;
			NN::Owned< N::CFragConnectionID >  itsFragmentConnection;
		
		public:
			CFMPluginMain( const BinaryImage& image ) : itsBinaryImage( image )
			{
			}
			
			void Invoke( int argc, iota::argv_t argv, iota::environ_t envp );
	};
	
	void CFMPluginMain::Invoke( int argc, iota::argv_t argv, iota::environ_t envp )
	{
		typedef void (*LampMain)( int, iota::argv_t, iota::environ_t );
		
		LampMain lamp_main = NULL;
		
		if ( itsFragmentConnection.Get() == N::CFragConnectionID() )
		{
			itsFragmentConnection = ConnectToFragment( itsBinaryImage );
			
			N::FindSymbol( itsFragmentConnection, "\p" "__lamp_main", &lamp_main );
		}
		
		ASSERT( lamp_main != NULL );
		
		lamp_main( argc, argv, envp );
	}
	
	
#if TARGET_RT_MAC_CFM
	
	typedef CFMPluginMain ExternalBinaryMain;
	
#else
	
	typedef CodeResourceMain ExternalBinaryMain;
	
#endif
	
	MainEntry GetMainEntryFromAddress( Main3 address )
	{
		return MainEntry( new AddressMain( address ) );
	}
	
	MainEntry GetMainEntryFromBinaryImage( const BinaryImage& binary )
	{
		return MainEntry( new ExternalBinaryMain( binary ) );
	}
	
}

