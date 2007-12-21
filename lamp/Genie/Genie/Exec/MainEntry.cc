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
	
	
	typedef MainEntryPoint::Main Main;
	
	
	MainEntryPoint::~MainEntryPoint()
	{
	}
	
	
	class InternalMain : public MainEntryPoint
	{
		public:
			InternalMain( Main main ) : MainEntryPoint( main, NULL, NULL )  {}
	};
	
	
	inline NN::Owned< N::CFragConnectionID > ConnectToFragment( const BinaryImage& image )
	{
		return N::GetMemFragment< N::kPrivateCFragCopy >( image.Get(),
		                                                  N::GetPtrSize( image ) );
	}
	
	
	class CodeResourceMain : public MainEntryPoint
	{
		private:
			BinaryImage itsBinaryImage;
		
		public:
			CodeResourceMain( const BinaryImage& image ) : MainEntryPoint( reinterpret_cast< Main >( image.Get().Get() ),
			                                                               NULL,
			                                                               NULL ),
			                                               itsBinaryImage( image )
			{
			}
	};
	
	
	class ConnectedFragment
	{
		private:
			Nucleus::Owned< N::CFragConnectionID > itsFragmentConnection;
		
		public:
			typedef MainEntryPoint::Environ Environ;
			
			ConnectedFragment( const BinaryImage& image ) : itsFragmentConnection( ConnectToFragment( image ) )
			{
				ImportSystemCalls( itsFragmentConnection );
			}
			
			Main GetMain() const
			{
				Main mainEntry = NULL;
				
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
			
			Environ* GetEnviron() const
			{
				Environ* environPtr = NULL;
				
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
	
	class CFMPluginMain : public ConnectedFragment, public MainEntryPoint
	{
		private:
			BinaryImage itsBinaryImage;
		
		public:
			CFMPluginMain( const BinaryImage& image );
	};
	
	
	CFMPluginMain::CFMPluginMain( const BinaryImage& image ) : ConnectedFragment( image        ),
	                                                           MainEntryPoint   ( GetMain   (),
	                                                                              GetErrno  (),
	                                                                              GetEnviron() ),
			                                                   itsBinaryImage   ( image        )
	{
	}
	
	
#if TARGET_RT_MAC_CFM
	
	typedef CFMPluginMain ExternalBinaryMain;
	
#else
	
	typedef CodeResourceMain ExternalBinaryMain;
	
#endif
	
	
	MainEntry GetMainEntryFromFile( const FSSpec& file )
	{
		BinaryImage binary = GetBinaryImage( file );
		
		return MainEntry( new ExternalBinaryMain( binary ) );
	}
	
	MainEntry GetMainEntryFromFile( const FSTreePtr& file )
	{
		// check for internally mapped programs, e.g. true, false, pwd, echo
		
		FSSpec programFile = file->GetFSSpec();
		
		return GetMainEntryFromFile( programFile );
	}
	
}

