/*	=======================
 *	FSTree_new_iconsuite.cc
 *	=======================
 */

#include "Genie/FileSystem/FSTree_new_iconsuite.hh"

// POSIX
#include <fcntl.h>

// Nitrogen
#include "Nitrogen/Icons.h"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_IconSuite.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"


namespace Pedestal
{
	
	class IconSuite : public View
	{
		public:
			void Draw( const Rect& bounds );
			
			virtual Nitrogen::IconSuiteRef Data() const = 0;
	};
	
	namespace N = Nitrogen;
	
	void IconSuite::Draw( const Rect& bounds )
	{
		if ( N::IconSuiteRef data = Data() )
		{
			N::EraseRect( bounds );
			
			N::PlotIconSuite( bounds, N::kAlignNone, N::kTransformNone, data );
		}
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	typedef std::map< const FSTree*, NN::Shared< N::IconSuiteRef > > IconSuiteMap;
	
	static IconSuiteMap gIconSuiteMap;
	
	
	class IconSuite : public Ped::IconSuite
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			IconSuite( Key key ) : itsKey( key )
			{
			}
			
			Nitrogen::IconSuiteRef Data() const;
	};
	
	N::IconSuiteRef IconSuite::Data() const
	{
		IconSuiteMap::const_iterator it = gIconSuiteMap.find( itsKey );
		
		if ( it != gIconSuiteMap.end() )
		{
			return it->second;
		}
		
		return N::IconSuiteRef();
	}
	
	boost::shared_ptr< Ped::View > IconSuiteFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new IconSuite( delegate ) );
	}
	
	
	void FSTree_new_iconsuite::DestroyDelegate( const FSTree* delegate )
	{
		gIconSuiteMap.erase( delegate );
	}
	
	
	static std::size_t GetHandleSize_Checked( N::Handle h )
	{
		return h ? N::GetHandleSize( h ) : 0;
	}
	
	class FSTree_IconSuite_ref : public FSTree
	{
		public:
			FSTree_IconSuite_ref( const FSTreePtr&    parent,
			                      const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			N::IconSuiteRef Data() const  { return gIconSuiteMap[ ParentRef().get() ]; }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			void Attach( const FSTreePtr& target ) const;
	};
	
	void FSTree_IconSuite_ref::Attach( const FSTreePtr& target ) const
	{
		gIconSuiteMap[ ParentRef().get() ] = Fetch_IconSuite();
	}
	
	
	const FSTree_Premapped::Mapping IconSuite_view_Mappings[] =
	{
		{ "ref", &Basic_Factory< FSTree_IconSuite_ref > },
		
		{ NULL, NULL }
	};
	
}

