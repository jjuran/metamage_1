/*	============
 *	MainEntry.cc
 *	============
 */

#include "Genie/Exec/MainEntry.hh"

// Nitrogen
#include "Nitrogen/CodeFragments.h"
#include "Nitrogen/Files.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	MainEntryPoint::~MainEntryPoint()
	{
	}
	
	
	class AddressMain : public MainEntryPoint
	{
		private:
			Main3 itsMain;
		
		public:
			AddressMain( Main3 main ) : itsMain( main )
			{
			}
			
			Main3 GetMainPtr()  { return itsMain; }
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
			Main3                              itsMain;
		
		public:
			CFMPluginMain( const BinaryImage& image ) : itsBinaryImage( image )
			{
			}
			
			Main3 GetMainPtr();
	};
	
	Main3 CFMPluginMain::GetMainPtr()
	{
		if ( itsFragmentConnection.Get() == N::CFragConnectionID() )
		{
			itsFragmentConnection = ConnectToFragment( itsBinaryImage );
			
			N::FindSymbol( itsFragmentConnection, "\p" "main", &itsMain );
		}
		
		return itsMain;
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
	
	MainEntry GetMainEntryFromFile( const FSSpec& file )
	{
		BinaryImage image = GetBinaryImage( file );
		
		return GetMainEntryFromBinaryImage( image );
	}
	
}

