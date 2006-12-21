/*	===================
 *	Vertice/PortView.hh
 *	===================
 */

#ifndef VERTICE_PORTVIEW_HH
#define VERTICE_PORTVIEW_HH

// Nitrogen
#include "Nitrogen/QDOffscreen.h"

// Nitrogen Extras / Utilities
#include "Utilities/Threads.h"

// Pedestal
#include "Pedestal/View.hh"

// Vertice
#include "Vertice/Port.hh"


namespace Vertice
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	namespace Ped = Pedestal;
	namespace V = Vectoria;
	
	class PortView;
	
	struct ThreadContext
	{
		const PortView* pane;
		
		ThreadContext()  {}
		
		ThreadContext( const PortView* pane )
		:
			pane( pane )
		{}
	};
	
	struct Nothing {};
	
	struct BetterDrawer
	{
		Nothing operator()( ThreadContext& context ) const;
	};
	
	struct Thread_Details
	{
		typedef ThreadContext  Parameter;
		typedef Nothing        Result;
		
		typedef BetterDrawer Function;
	};
	
	typedef NX::Thread< Thread_Details > Thread;
	
	class PortView : public Ped::View
	{
		private:
			//std::pair<std::string, int> HitTest(double x, double y);
			MeshPoly* HitTest( double x, double y );
			Vertice::ColorMatrix TracePixel( int x, int y );
			void DrawPixel( int x, int y );
			
			Rect myBounds;  // port coordinates
			Model myModel;
			Port myPort;
			Frame myFrame;
			V::XMatrix myScreen2Port;
			std::size_t mySelectedContext;
			NN::Owned< N::GWorldPtr > myGWorld;
			std::auto_ptr< Thread > drawBetterThread;
		
		public:
			PortView( const Rect& bounds, Initializer );
			~PortView()  {}
			
			Model& ItsModel()  { return myModel; }
			
			// Pane management
			const Rect& Bounds() const  { return myBounds; }
			void SetBounds( const Rect& bounds );
			
			// Event processing
			bool DispatchCursor( const EventRecord& event );
			void MouseDown( const EventRecord& event );
			bool KeyDown( const EventRecord& event );
			bool KeyDown( char c );
			bool DoCommand( Ped::MenuItemCode code )  { return false; }
			void Resize( const Rect& newBounds );
			void Draw();
			void DrawBetter() const;
			
			void Update()  { Draw(); }
	};
	
}

#endif

