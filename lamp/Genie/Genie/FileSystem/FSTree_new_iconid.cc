/*	====================
 *	FSTree_new_iconid.cc
 *	====================
 */

#include "Genie/FileSystem/FSTree_new_iconid.hh"

// Nitrogen
#include "Nitrogen/Icons.h"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"


namespace Pedestal
{
	
	class IconID : public View
	{
		public:
			struct Undefined {};
			
			void Draw( const Rect& bounds );
			
			virtual Nitrogen::ResID ID() const = 0;
	};
	
	namespace N = Nitrogen;
	
	void IconID::Draw( const Rect& bounds )
	{
		N::EraseRect( bounds );
		
		try
		{
			N::PlotIconID( bounds, N::kAlignNone, N::kTransformNone, ID() );
		}
		catch ( const Undefined& )
		{
		}
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	typedef std::map< const FSTree*, N::ResID > IconIDMap;
	
	static IconIDMap gIconIDMap;
	
	
	class IconID : public Ped::IconID
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			IconID( Key key ) : itsKey( key )
			{
			}
			
			Nitrogen::ResID ID() const;
	};
	
	N::ResID IconID::ID() const
	{
		IconIDMap::const_iterator it = gIconIDMap.find( itsKey );
		
		if ( it == gIconIDMap.end() )
		{
			throw Undefined();
		}
		
		return it->second;
	}
	
	std::auto_ptr< Ped::View > IconIDFactory( const FSTree* delegate )
	{
		return std::auto_ptr< Ped::View >( new IconID( delegate ) );
	}
	
	
	void FSTree_new_iconid::DestroyDelegate( const FSTree* delegate )
	{
		gIconIDMap.erase( delegate );
	}
	
	
	static std::string ReadIconID( const FSTree* view )
	{
		return NN::Convert< std::string >( gIconIDMap[ view ] );
	}
	
	static void WriteIconID( const FSTree* view, const char* begin, const char* end )
	{
		// *end == '\n'
		
		gIconIDMap[ view ] = N::ResID( std::atoi( begin ) );
		
		InvalidateWindowForView( view );
	}
	
	
	static FSTreePtr IDFactory( const FSTreePtr&    parent,
	                            const std::string&  name )
	{
		return FSTreePtr( new FSTree_Mutable_Property( parent,
		                                               name,
		                                               &ReadIconID,
		                                               &WriteIconID ) );
	}
	
	const Functional_Traits< void >::Mapping IconID_view_Mappings[] =
	{
		{ "id", &IDFactory },
		
		{ NULL, NULL }
	};
	
}

