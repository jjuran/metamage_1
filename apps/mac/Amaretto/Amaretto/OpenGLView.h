/*
	Amaretto/OpenGLView.h
	---------------------
*/

// Mac OS X
#import <Cocoa/Cocoa.h>


@interface AmarettoOpenGLView : NSOpenGLView
{
	CGSize  _resolution;
	CGFloat _scale;
}

- (id) initWithResolution: (CGSize) resolution;

- (void) setScale: (CGFloat) scale;

- (BOOL) acceptsFirstResponder;
- (BOOL) isFlipped;
- (BOOL) isOpaque;

- (void) reshape;

- (void) drawRect: (NSRect) bounds;

- (void) handleMouseMovedTo: (NSPoint) location;

- (void) mouseDragged: (NSEvent*) event;
- (void) mouseMoved:   (NSEvent*) event;
- (void) mouseDown:    (NSEvent*) event;
- (void) mouseUp:      (NSEvent*) event;
- (void) keyDown:      (NSEvent*) event;
- (void) keyUp:        (NSEvent*) event;
- (void) flagsChanged: (NSEvent*) event;

@end
