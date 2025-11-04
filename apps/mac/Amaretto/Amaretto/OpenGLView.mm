/*
	Amaretto/OpenGLView.mm
	----------------------
*/

#include "Amaretto/OpenGLView.h"

// OpenGL
#include <OpenGL/gl.h>

// write-a-splode
#include "splode/write-a-splode.hh"

// glfb-common
#include "glfb/glfb.hh"

// amicus
#include "amicus/events.hh"


static bool cursor_hidden;

static
void handle_event( NSEvent* event )
{
	amicus::handle_CGEvent( [event CGEvent] );
}

@implementation AmarettoOpenGLView

- (id) initWithResolution: (CGSize) resolution
{
	[super init];
	
	[[self openGLContext] makeCurrentContext];
	
#ifdef MAC_OS_X_VERSION_10_7
	
	[self setWantsBestResolutionOpenGLSurface: YES];
	
#endif
	
	_resolution = resolution;
	_scale      = 1;
	
	return self;
}

- (void) setScale: (CGFloat) scale
{
	_scale = scale;
	
	CGFloat width  = _resolution.width  * _scale;
	CGFloat height = _resolution.height * _scale;
	
	glViewport( 0, 0, (long) width, (long) height );
	
	NSSize size = NSMakeSize( width, height );
	
	NSWindow* window = [self window];
	
	[window setContentSize: size];
	[self   setFrameSize:   size];
}

- (BOOL) acceptsFirstResponder
{
	return YES;
}

- (BOOL) isFlipped
{
	return YES;
}

- (BOOL) isOpaque
{
	return YES;
}

- (void) drawRect: (NSRect) bounds
{
	glfb::render();
	
	glFlush();
}

- (void) handleMouseMovedEvent: (NSEvent*) event
{
	NSPoint pt = [self convertPoint: [event locationInWindow] fromView: nil];
	
	if ( [self hitTest: pt] )
	{
		if ( ! cursor_hidden )
		{
			cursor_hidden = true;
			
			CGDisplayHideCursor( 0 );
		}
		
		CGFloat x = pt.x / _scale;
		CGFloat y = pt.y / _scale;
		
		splode::send_mouse_moved_event( amicus::events_fd, (int) x, (int) y );
	}
	else if ( cursor_hidden )
	{
		cursor_hidden = false;
		
		CGDisplayShowCursor( 0 );
	}
}

- (void) handleMouseEvent: (NSEvent*) event
{
	[self handleMouseMovedEvent: event];
	
	handle_event( event );
}

- (void) mouseDragged: (NSEvent*) event
{
	[self handleMouseMovedEvent: event];
}

- (void) mouseMoved: (NSEvent*) event
{
	[self handleMouseMovedEvent: event];
}

- (void) mouseDown: (NSEvent*) event
{
	[self handleMouseEvent: event];
}

- (void) mouseUp: (NSEvent*) event
{
	[self handleMouseEvent: event];
}

- (void) keyDown: (NSEvent*) event
{
	handle_event( event );
}

- (void) keyUp: (NSEvent*) event
{
	handle_event( event );
}

- (void) flagsChanged: (NSEvent*) event
{
	handle_event( event );
}

@end
