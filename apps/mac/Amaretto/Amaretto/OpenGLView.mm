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
#include "amicus/resize.hh"


using amicus::get_proportional_coordinate;
using amicus::get_proportional_offset;


static bool cursor_hidden;

static
NSPoint get_proportional_location( NSRect bounds )
{
	NSPoint origin = bounds.origin;
	NSSize  window = bounds.size;
	
	NSSize space = [[NSScreen mainScreen] frame].size;
	
	return NSMakePoint
	(
		get_proportional_coordinate( space.width,  window.width,  origin.x ),
		get_proportional_coordinate( space.height, window.height, origin.y )
	);
}

static
NSPoint new_location( NSSize size, NSPoint location )
{
	NSSize space = [[NSScreen mainScreen] frame].size;
	
	return NSMakePoint
	(
		get_proportional_offset( space.width,  size.width,  location.x ),
		get_proportional_offset( space.height, size.height, location.y )
	);
}

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
	
	CGFloat inner_width  = _resolution.width  * _scale;
	CGFloat inner_height = _resolution.height * _scale;
	
	glViewport( 0, 0, (long) inner_width, (long) inner_height );
	
	NSSize inner_size = NSMakeSize( inner_width, inner_height );
	NSSize outer_size = inner_size;
	
	NSWindow* window = [self window];
	
	NSPoint proportional_location = get_proportional_location( [window frame] );
	
	proportional_location = new_location( outer_size, proportional_location );
	
	NSDisableScreenUpdates();
	
	[window setContentSize: outer_size];
	[window setFrameOrigin: proportional_location];
	
	[self   setFrameSize:   inner_size];
	
	NSEnableScreenUpdates();
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
