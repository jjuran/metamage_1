/*
	Vertice/PortView.hh
	-------------------
*/

#ifndef VERTICE_PORTVIEW_HH
#define VERTICE_PORTVIEW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_7
#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QDOFFSCREEN_H
#include "missing/QDOffscreen.h"
#endif
#endif

// mac-config
#include "mac_config/compositing-mode.hh"

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
			GWorldPtr                    itsGWorld;
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
