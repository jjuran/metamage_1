/*
	Vertice/PortView.cc
	-------------------
*/

#include "Vertice/PortView.hh"

// Standard C
#include <string.h>

// Standard C++
#include <vector>

// mac-cg-utils
#include "mac_cg/images.hh"

// mac-qd-utils
#include "mac_qd/copy_bits.hh"
#include "mac_qd/globals/thePort_window.hh"
#include "mac_qd/set_cursor_by_id.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// Nitrogen
#ifndef NITROGEN_QDOFFSCREEN_HH
#include "Nitrogen/QDOffscreen.hh"
#endif

// worldview
#include "worldview/Render.hh"


#ifndef MAC_OS_X_VERSION_10_4
	
	typedef CGImageAlphaInfo CGBitmapInfo;
	
#endif


namespace Vertice
{
	
	PortView::PortView( const Rect& bounds ) : itsBounds         ( bounds      ),
	                                           itsPort           ( itsScene    ),
	                                           itsSelectedContext(             ),
	                                           itsAnaglyphMode   ( kNoAnaglyph )
	{
		itsGWorld = NULL;
		
	#if CONFIG_COMPOSITING
		
		itsImage = NULL;
		
	#endif
		
		SetBounds( bounds );
	}
	
	PortView::~PortView()
	{
	#if CONFIG_COMPOSITING
		
		if ( itsImage )
		{
			CGImageRelease( itsImage );
		}
		
	#endif
		
		if ( itsGWorld )
		{
			DisposeGWorld( itsGWorld );
		}
	}
	
	
	static
	void render_into_GWorld( const std::vector< MeshModel >&  models,
	                         render_proc                      render,
	                         GWorldPtr                        gworld )
	{
		PixMapHandle pix = GetGWorldPixMap( gworld );
		
		const Rect  bounds = pix[0]->bounds;
		::Ptr       base   = pix[0]->baseAddr;
		unsigned    stride = pix[0]->rowBytes & 0x3fff;
		
		const unsigned width  = bounds.right - bounds.left;
		const unsigned height = bounds.bottom - bounds.top;
		
		memset( base, '\0', height * stride );
		
		render( &*models.begin(), &*models.end(), base, width, height, stride );
	}
	
	static
	CGImageRef CGImage_from_GWorld( CGrafPtr gworld )
	{
		using mac::cg::create_xRGB_8888_image;
		
		PixMapHandle pix = ::GetPortPixMap( gworld );
		
		const Rect  bounds = pix[0]->bounds;
		::Ptr       base   = pix[0]->baseAddr;
		unsigned    stride = pix[0]->rowBytes & 0x3fff;
		
		const size_t width  = bounds.right - bounds.left;
		const size_t height = bounds.bottom - bounds.top;
		
		static CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
		
		CGImageRef image = create_xRGB_8888_image( width,
		                                           height,
		                                           stride,
		                                           colorSpace,
		                                           base );
		
		return image;
	}
	
	static
	GWorldPtr new_GWorld( Rect bounds )
	{
	#ifdef MAC_OS_X_VERSION_10_7
		
		WindowRef window = mac::qd::thePort_window();
		
		CGFloat factor = HIWindowGetBackingScaleFactor( window );
		
		bounds.right *= factor;
		bounds.bottom *= factor;
		
	#endif
		
		QDErr err;
		GWorldPtr gworld;
		
		err = NewGWorld( &gworld, 32, &bounds, NULL, NULL, GWorldFlags() );
		
		if ( err )
		{
			return NULL;
		}
		
		LockPixels( GetGWorldPixMap( gworld ) );  // pixels not purgeable
		
		return gworld;
	}
	
	void PortView::Render()
	{
		if ( ! itsGWorld )
		{
			return;
		}
		
		if ( itsAnaglyphMode )
		{
			DrawAnaglyphic();
		}
		else
		{
			itsPort.MakeFrame( itsFrame );
			
			render_into_GWorld( itsFrame.Models(), &paint_onto_surface, itsGWorld );
		}
		
		Derive();
	}
	
	void PortView::Derive() const
	{
	#if CONFIG_COMPOSITING
		
		if ( CGImageRef image = CGImage_from_GWorld( itsGWorld ) )
		{
			if ( itsImage )
			{
				CGImageRelease( itsImage );
			}
			
			itsImage = image;
		}
		
	#endif
	}
	
	void PortView::Update() const
	{
	#if CONFIG_COMPOSITING
		
		WindowRef window = mac::qd::thePort_window();
		
		if ( mac::ui::invalidate_if_compositing( window ) )
		{
			return;
		}
		
	#endif
		
		mac::qd::copy_bits( itsGWorld, mac::qd::thePort() );
	}
	
	void PortView::Draw( const Rect& bounds, bool erasing )
	{
		Update();
	}
	
	void PortView::DrawInContext( CGContextRef context, CGRect bounds )
	{
	#if CONFIG_COMPOSITING
		
		OSStatus err;
		
		if ( itsImage )
		{
			err = HIViewDrawCGImage( context, &bounds, itsImage );
		}
		
	#endif
	}
	
	void PortView::DrawAnaglyphic()
	{
		std::size_t contextIndex = itsScene.Cameras().front();
		
		Moveable& target = itsPort.itsScene.GetContext( contextIndex );
		
		double eyeRadius = 0.05;  // distance from eye to bridge of nose
		
		nucleus::owned< GWorldPtr > altGWorld;
		
		if ( GWorldPtr gworld = new_GWorld( itsBounds ) )
		{
			altGWorld = nucleus::owned< GWorldPtr >::seize( gworld );
		}
		else
		{
			return;
		}
		
		
		target.ContextTranslate( -eyeRadius, 0, 0 );
		
		itsPort.MakeFrame( itsFrame );
		
		render_into_GWorld( itsFrame.Models(), &paint_onto_surface, altGWorld );
		
		
		target.ContextTranslate( 2 * eyeRadius, 0, 0 );
		
		itsPort.MakeFrame( itsFrame );
		
		render_into_GWorld( itsFrame.Models(), &paint_onto_surface, itsGWorld );
		
		
		target.ContextTranslate( -eyeRadius, 0, 0 );
		
		PixMapHandle pixL = GetGWorldPixMap( altGWorld );
		PixMapHandle pixR = GetGWorldPixMap( itsGWorld );
		
		::Ptr baseL = pixL[0]->baseAddr;
		::Ptr baseR = pixR[0]->baseAddr;
		
		const Rect bounds = pixL[0]->bounds;
		
		unsigned width  = bounds.right - bounds.left;
		unsigned height = bounds.bottom - bounds.top;
		
		unsigned stride = pixL[0]->rowBytes & 0x3fff;
		
		merge_anaglyph( itsAnaglyphMode, baseL, baseR, height, width, stride );
	}
	
	
	bool PortView::DispatchCursor( const EventRecord& event )
	{
		mac::qd::set_cursor_by_id( crossCursor );
		
		return true;
	}
	
	void PortView::DrawBetter() const
	{
		render_into_GWorld( itsFrame.Models(), &trace_onto_surface, itsGWorld );
		
		Derive();
		Update();
	}
	
	bool PortView::MouseDown( const EventRecord& event )
	{
		Point macPt = event.where;
		
		GlobalToLocal( &macPt );
		
		itsPort.MakeFrame( itsFrame );
		
		const int width  = itsBounds.right - itsBounds.left;
		const int height = itsBounds.bottom - itsBounds.top;
		
		const double x = (macPt.h + 0.5 - width  / 2.0) / (width /  2.0);
		const double y = (macPt.v + 0.5 - height / 2.0) / (width / -2.0);
		
		MeshModel* model = hit_test( itsFrame, x, y );
		
		if ( model != NULL )
		{
			model->Select();
		}
		
		render_into_GWorld( itsFrame.Models(), &paint_onto_surface, itsGWorld );
		
		Derive();
		Update();
		
		return true;
	}
	
	bool PortView::KeyDown( const EventRecord& event )
	{
		return KeyDown( event.message );
	}
	
	bool PortView::KeyDown( char c )
	{
		if ( c == '~' )
		{
			DrawBetter();
			
			return true;
		}
		
		short cmd = cmdNone;
		
		bool shooter = true;
		if ( shooter )
		{
			switch ( c )
			{
				case 0x1e: // up arrow
					c = '0';
				break;
				case 0x1f: // down arrow
					c = '.';
				break;
			}
		}
		else
		{
			switch ( c )
			{
				case 0x1e: // up arrow
					c = '8';
					break;
				case 0x1f: // down arrow
					c = '5';
					break;
			}
		}
		switch ( c )
		{
			case 'a':
				itsAnaglyphMode = kNoAnaglyph;
				break;
			
			case 's':
				itsAnaglyphMode = kTrueAnaglyph;
				break;
			
			case 'd':
				itsAnaglyphMode = kGrayAnaglyph;
				break;
			
			case 'f':
				itsAnaglyphMode = kColorAnaglyph;
				break;
			
			case 'g':
				itsAnaglyphMode = kHalfColorAnaglyph;
				break;
			
			case 'h':
				itsAnaglyphMode = kOptimizedAnaglyph;
				break;
			
			case '7':
				cmd = cmdMoveLeft;
				break;
			case '9':
				cmd = cmdMoveRight;
				break;
			case '4':
			case 0x1c: // left arrow
				cmd = cmdYawLeft;
				break;
			case '6':
			case 0x1d: // right arrow
				cmd = cmdYawRight;
				break;
			case '1':
				cmd = cmdRollLeft;
				break;
			case '3':
				cmd = cmdRollRight;
				break;
			case '8':
				cmd = cmdPitchDown;
				break;
			case '5':
				cmd = cmdPitchUp;
				break;
			case '0':
				cmd = cmdMoveForward;
				break;
			case '.':
				cmd = cmdMoveBackward;
				break;
			case '-':
				cmd = cmdMoveUp;
				break;
			case '+':
				cmd = cmdMoveDown;
				break;
			case '*':
				cmd = cmdExpand;
				break;
			case '/':
				cmd = cmdContract;
				break;
			case '2':
				cmd = cmdLevelPitch;
				break;
			case '=':
				cmd = cmdLevelRoll;
				break;
			case '_':
			case 0x03:
				cmd = cmdGroundHeight;
				break;
			case ' ':
				cmd = cmdPlayPause;
				break;
			case 'p':
				//cmd = cmdProjectionMode;
				break;
			case '[':
				cmd = cmdPrevCamera;
				break;
			case ']':
				cmd = cmdNextCamera;
				break;
			default:
				return false;
				break;
		}
		
		if ( itsSelectedContext == 0 )
		{
			itsSelectedContext = itsScene.Cameras().front();
		}
		
		itsPort.SendCameraCommand( itsSelectedContext, cmd );
		
		Render();
		Update();
		
		return true;
	}
	
	void PortView::SetBounds( const Rect& bounds )
	{
		itsBounds = bounds;
		
		if ( itsGWorld )
		{
			DisposeGWorld( itsGWorld );
		}
		
		itsGWorld = new_GWorld( itsBounds );
		
		Render();
	}
	
}
