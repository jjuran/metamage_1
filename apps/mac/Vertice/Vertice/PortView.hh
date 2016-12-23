/*
	Vertice/PortView.hh
	-------------------
*/

#ifndef VERTICE_PORTVIEW_HH
#define VERTICE_PORTVIEW_HH

// Nitrogen
#ifndef NITROGEN_QDOFFSCREEN_HH
#include "Nitrogen/QDOffscreen.hh"
#endif

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif

// Vertice
#include "Vertice/Anaglyphs.hh"
#include "Vertice/Port.hh"


namespace Vertice
{
	
	class PortView : public Pedestal::View
	{
		private:
			MeshModel* Mesh_HitTest( double x, double y );
			
			Vertice::ColorMatrix TracePixel( int x, int y );
			
			void DrawPixel( int x, int y );
			
			Rect                         itsBounds;  // port coordinates
			Scene                        itsScene;
			Port                         itsPort;
			Frame                        itsFrame;
			std::size_t                  itsSelectedContext;
			nucleus::owned< GWorldPtr >  itsGWorld;
			anaglyph_mode                itsAnaglyphMode;
		
		public:
			PortView( const Rect& bounds );
			~PortView()  {}
			
			Scene& ItsScene()  { return itsScene; }
			
			// Pane management
			const Rect& Bounds() const  { return itsBounds; }
			
			// Event processing
			bool DispatchCursor( const EventRecord& event );
			bool MouseDown( const EventRecord& event );
			bool KeyDown( const EventRecord& event );
			bool KeyDown( char c );
			bool DoCommand( Pedestal::CommandCode code )  { return false; }
			void SetBounds( const Rect& bounds );
			void Paint();
			void Redraw();
			void Draw( const Rect& bounds, bool erasing );
			void DrawAnaglyphic();
			void DrawBetter( bool per_scanline ) const;
	};
	
}

#endif
