/*	===================
 *	Vertice/PortView.hh
 *	===================
 */

#ifndef VERTICE_PORTVIEW_HH
#define VERTICE_PORTVIEW_HH

// Nitrogen
#include "Nitrogen/QDOffscreen.h"

// Pedestal
#include "Pedestal/View.hh"

// Vertice
#include "Vertice/Port.hh"


namespace Vertice
{
	
	class PortView : public Pedestal::View
	{
		private:
			MeshPoly* HitTest( double x, double y );
			Vertice::ColorMatrix TracePixel( int x, int y );
			void DrawPixel( int x, int y );
			
			Rect                                   itsBounds;  // port coordinates
			Scene                                  itsScene;
			Port                                   itsPort;
			Frame                                  itsFrame;
			Vectoria::XMatrix                      itsScreen2Port;
			std::size_t                            itsSelectedContext;
			Nucleus::Owned< Nitrogen::GWorldPtr >  itsGWorld;
		
		public:
			PortView( const Rect& bounds, Initializer );
			~PortView()  {}
			
			Scene& ItsScene()  { return itsScene; }
			
			// Pane management
			const Rect& Bounds() const  { return itsBounds; }
			void SetBounds( const Rect& bounds );
			
			// Event processing
			bool DispatchCursor( const EventRecord& event );
			void MouseDown( const EventRecord& event );
			bool KeyDown( const EventRecord& event );
			bool KeyDown( char c );
			bool DoCommand( Pedestal::MenuItemCode code )  { return false; }
			void Resize( const Rect& newBounds );
			void Draw();
			void DrawBetter() const;
			
			void Update()  { Draw(); }
	};
	
}

#endif

