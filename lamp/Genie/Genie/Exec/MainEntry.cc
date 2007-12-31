/*	============
 *	MainEntry.cc
 *	============
 */

#include "Genie/Exec/MainEntry.hh"

// Nitrogen
#include "Nitrogen/CodeFragments.h"
#include "Nitrogen/Files.h"

// Genie
#include "Genie/BinaryImage.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	extern "C" void DispatchSystemCall();
	
	
	MainEntryPoint::~MainEntryPoint()
	{
	}
	
	
	class BinaryImageClient
	{
		private:
			BinaryImage itsBinaryImage;
		
		public:
			BinaryImageClient( const BinaryImage& image ) : itsBinaryImage( image )
			{
			}
	};
	
	class CodeResourceMain : public BinaryImageClient, public MainEntryPoint
	{
		public:
			CodeResourceMain( const BinaryImage& image ) : BinaryImageClient( image ),
			                                               MainEntryPoint( reinterpret_cast< Main3 >( image.Get().Get() ),
			                                                               NULL,
			                                                               NULL )
			{
			}
	};
	
	
	inline NN::Owned< N::CFragConnectionID > ConnectToFragment( const BinaryImage& image )
	{
		return N::GetMemFragment< N::kPrivateCFragCopy >( image.Get(),
		                                                  N::GetPtrSize( image ) );
	}
	
	
	static void LoadSymbol( N::CFragConnectionID fragment, ConstStr255Param symName, void* value )
	{
		void** symbol = NULL;
		
		N::FindSymbol( fragment, symName, &symbol );
		
		*symbol = value;
	}
	
	class ConnectedFragment
	{
		private:
			Nucleus::Owned< N::CFragConnectionID > itsFragmentConnection;
		
		public:
			ConnectedFragment( const BinaryImage& image ) : itsFragmentConnection( ConnectToFragment( image ) )
			{
				try
				{
					LoadSymbol( itsFragmentConnection,
					            "\p" "gDispatcher",
					            DispatchSystemCall );
				}
				catch ( ... )
				{
					// dispatcher optional if you don't make system calls
				}
			}
			
			Main3 GetMain() const
			{
				Main3 mainEntry = NULL;
				
				N::FindSymbol( itsFragmentConnection, "\p" "main", &mainEntry );
				
				return mainEntry;
			}
			
			int* GetErrno() const
			{
				int* errnoPtr = NULL;
				
				try
				{
					N::FindSymbol( itsFragmentConnection, "\p" "errno", &errnoPtr );
				}
				catch ( ... )
				{
				}
				
				return errnoPtr;
			}
			
			iota::environ_t* GetEnviron() const
			{
				iota::environ_t* environPtr = NULL;
				
				try
				{
					N::FindSymbol( itsFragmentConnection, "\p" "environ", &environPtr );
				}
				catch ( ... )
				{
				}
				
				return environPtr;
			}
	};
	
	class CFMPluginMain : public BinaryImageClient, public ConnectedFragment, public MainEntryPoint
	{
		public:
			CFMPluginMain( const BinaryImage& image );
	};
	
	
	CFMPluginMain::CFMPluginMain( const BinaryImage& image ) : BinaryImageClient( image        ),
	                                                           ConnectedFragment( image        ),
	                                                           MainEntryPoint   ( GetMain   (),
	                                                                              GetErrno  (),
	                                                                              GetEnviron() )
	{
	}
	
	
#if TARGET_RT_MAC_CFM
	
	typedef CFMPluginMain ExternalBinaryMain;
	
#else
	
	typedef CodeResourceMain ExternalBinaryMain;
	
#endif
	
	MainEntry GetMainEntryFromAddress( Main3 address )
	{
		return MainEntry( new MainEntryPoint( address ) );
	}
	
	MainEntry GetMainEntryFromFile( const FSSpec& file )
	{
		BinaryImage binary = GetBinaryImage( file );
		
		return MainEntry( new ExternalBinaryMain( binary ) );
	}
	
}

