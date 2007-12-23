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
	
	
	template < class Type >
	bool LoadSymbol( N::CFragConnectionID fragment, ConstStr255Param symName, const Type& init )
	{
		
		try
		{
			Type* symbol = NULL;
			
			N::FindSymbol( fragment, symName, &symbol );
			
			*symbol = init;
		}
		catch ( ... )
		{
			return false;
		}
		
		return true;
	}
	
	class SymbolImporter
	{
		private:
			N::CFragConnectionID itsFragmentConnection;
		
		public:
			SymbolImporter( N::CFragConnectionID conn ) : itsFragmentConnection( conn )  {}
			
			bool operator()( SystemCallRegistry::value_type systemCall ) const
			{
				std::string name = systemCall.name;
				void*       func = systemCall.function;
				
				name += "_import_";
				
				return LoadSymbol( itsFragmentConnection, N::Str255( name ), func );
			}
	};
	
	static void ImportSystemCalls( N::CFragConnectionID fragmentConnection )
	{
		std::for_each( SystemCallsBegin(),
		               SystemCallsEnd(),
		               SymbolImporter( fragmentConnection ) );
	}
	
	
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
	
	
	class ConnectedFragment
	{
		private:
			Nucleus::Owned< N::CFragConnectionID > itsFragmentConnection;
		
		public:
			ConnectedFragment( const BinaryImage& image ) : itsFragmentConnection( ConnectToFragment( image ) )
			{
				ImportSystemCalls( itsFragmentConnection );
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

