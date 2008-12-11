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
	
	
	class Caption : public Ped::Caption
	{
		public:
			struct Initializer {};
			
			Caption( Initializer = Initializer() )
			{
			}
			
			std::string Text() const  { return "Hello world"; }
	};
	
	struct CaptionFactory : ViewFactory
	{
		typedef Ped::GraphicView< Caption > View;
		
		void operator()( Ped::UserWindow& window ) const
		{
			Rect bounds = N::GetPortBounds( N::GetQDGlobalsThePort() );
			
			std::auto_ptr< Ped::View > view( new View( bounds ) );
			
			window.SetView( view );
			
			N::InvalRect( bounds );
		}
	};
	
	void FSTree_new_caption::HardLink( const FSTreePtr& target ) const
	{
		static CaptionFactory factory;
		
		const FSTree* key = target->Parent().get();
		
		FSTreePtr delegate = Premapped_Factory< Caption_view_Mappings >( target, "view" );
		
		AddViewDelegate( key, delegate );
		
		AddViewFactory( key, factory );
		
		target->SetTimes();
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

