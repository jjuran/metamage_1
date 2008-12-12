/*	=====================
 *	FSTree_new_caption.cc
 *	=====================
 */

#include "Genie/FileSystem/FSTree_new_caption.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Pedestal
#include "Pedestal/Caption.hh"
#include "Pedestal/GraphicView.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	extern const Functional_Traits< void >::Mapping Caption_view_Mappings[];
	
	typedef std::map< const FSTree*, std::string > CaptionTextMap;
	
	static CaptionTextMap gCaptionTextMap;
	
	
	class Caption : public Ped::Caption
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			Caption( Key key ) : itsKey( key )
			{
			}
			
			std::string Text() const;
	};
	
	std::string Caption::Text() const
	{
		std::string result;
		
		CaptionTextMap::const_iterator it = gCaptionTextMap.find( itsKey );
		
		if ( it != gCaptionTextMap.end() )
		{
			result = it->second;
		}
		
		std::replace( result.begin(), result.end(), '\n', '\r' );
		
		return result;
	}
	
	
	class CaptionFactory : public ViewFactory
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			CaptionFactory( Key key ) : itsKey( key )
			{
			}
			
			void operator()( Ped::UserWindow& window ) const
			{
				Rect bounds = N::GetPortBounds( N::GetQDGlobalsThePort() );
				
				typedef Ped::GraphicView< Caption > View;
				
				std::auto_ptr< Ped::View > view( new View( bounds, itsKey ) );
				
				window.SetView( view );
				
				N::InvalRect( bounds );
			}
	};
	
	namespace
	{
		
		void DestroyDelegate( const FSTree* delegate )
		{
			gCaptionTextMap.erase( delegate );
		}
		
	}
	
	void FSTree_new_caption::HardLink( const FSTreePtr& target ) const
	{
		const FSTreePtr& parent = target->Parent();
		
		const FSTree* key = parent.get();
		
		FSTreePtr delegate = Premapped_Factory< Caption_view_Mappings, &DestroyDelegate >( parent, "view" );
		
		boost::shared_ptr< ViewFactory > factory( new CaptionFactory( delegate.get() ) );
		
		AddViewDelegate( key, delegate );
		
		AddViewFactory( key, factory );
		
		target->CreateDirectory( 0 );  // mode is ignored
	}
	
	
	typedef FSTree Caption_text;
	
	
	
	template < class FSTree_Type >
	static FSTreePtr Factory( const FSTreePtr&    parent,
	                          const std::string&  name )
	{
		return MakeFSTree( new FSTree_Type( parent, name ) );
	}
	
	const Functional_Traits< void >::Mapping Caption_view_Mappings[] =
	{
		{ "text", &Factory< Caption_text > },
		
		{ NULL, NULL }
	};
	
}

