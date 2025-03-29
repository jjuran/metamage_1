/*
	Vertice/PortView.hh
	-------------------
*/

#ifndef VERTICE_PORTVIEW_HH
#define VERTICE_PORTVIEW_HH

// mac-config
#include "mac_config/compositing-mode.hh"

// Nitrogen
#ifndef NITROGEN_QDOFFSCREEN_HH
#include "Nitrogen/QDOffscreen.hh"
#endif

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif

// worldview
#include "worldview/Anaglyphs.hh"
#include "worldview/Port.hh"


namespace Vertice
{
	
	using namespace worldview;
	
	class PortView : public Pedestal::View
	{
		private:
			Rect                         itsBounds;  // port coordinates
			Scene                        itsScene;
			Port                         itsPort;
			Frame                        itsFrame;
			std::size_t                  itsSelectedContext;
			nucleus::owned< GWorldPtr >  itsGWorld;
		#if CONFIG_COMPOSITING
			mutable CGImageRef           itsImage;
		#endif
			anaglyph_mode                itsAnaglyphMode;
			
			// non-copyable
			PortView           ( const PortView& );
			PortView& operator=( const PortView& );
		
		public:
			PortView( const Rect& bounds );
			~PortView();
			
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
			void Render();
			void Derive() const;
			void Update() const;
			void Draw( const Rect& bounds, bool erasing );
			void DrawInContext( CGContextRef context, CGRect bounds );
			void DrawAnaglyphic();
			void DrawBetter() const;
	};
	
}

#endif
