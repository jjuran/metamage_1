/*
	Drag.hh
	-------
*/

#ifndef DRAG_HH
#define DRAG_HH

struct MacRegion;
struct Point;
struct Rect;

typedef pascal void (*action_proc)();

pascal long DragGrayRgn_patch( MacRegion**  rgn,
                               Point        start,
                               const Rect*  limit,
                               const Rect*  slop,
                               short        axis,
                               action_proc  action );

pascal long DragTheRgn_patch( MacRegion**  rgn,
                              Point        start,
                              const Rect*  limit,
                              const Rect*  slop,
                              short        axis,
                              action_proc  action );

#endif
