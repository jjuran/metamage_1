ams-qd:  Advanced Mac Substitute Basic Graphics Kit
===================================================

Depends on:

  * [ams-rsrc][]

Fixed Point Math
----------------

  * _FixMul, _FixRatio, _FixRound, _FixDiv (all natively accelerated)
  * _LongMul (TODO: accelerate)
  * _SlopeFromAngle, _AngleFromSlope

PRNG
----

  * _Random (natively accelerated)

Byte Coding
-----------

  * _PackBits, _UnpackBits (TODO: accelerate if determined a bottlneck)
  * _StuffHex

Geometry
--------

  * Point, Rect, and Region calculations
    * _PinRect (described in Window Manager, but belongs here)
    * _MapPt, _MapRect, _MapRgn
    * _InsetRgn
    * _XorRgn, _SectRgn, _UnionRgn, _DiffRgn
    * region operations in general are natively accelerated
  * reduction of more abstract shapes to regions
    * ovals (i.e. ellipses)
      * circles (pixel perfect)
        * TODO: memoize some circular regions? (WIP)
      * non-circular ovals (approximate)
      * TODO: natively accelerate?
    * round rects (circular corners perfect, otherwise approximate)
    * polygons (natively accelerated)
    * diagonal lines (TODO: natively accelerate?)
    * arcs / wedges (via ovals and polygons; TODO: accelerate further?)
  * _BitMapToRegion

Graphics Ports
--------------

  * polygon construction (TODO: may need work)
  * region construction (TODO: may need work)
  * picture construction (TODO: definitely needs work)
  * partial _SetStdProcs support (TODO)

Blitting
--------

  * _CopyBits
    * buffers as needed for operations to/from the same bitmap
    * stretching
  * _ScrollRect
  * _CopyMask (currently calls BitMapToRegion() and CopyBits()
    * this is slow and contrary to Inside Mac (TODO: refactor)

Font Manager
------------

  * supports 'FONT', 'FOND', and 'NFNT' resources
  * _RealFont
  * TODO: synthesize and cache styled font variants
  * TODO: synthesize and cache shifted font bitmaps?

Drawing
-------

  * frame/paint/erase/invert/fill of rects and regions
    * "regions" covers the more abstract shapes, by reduction
  * _FramePoly (correctly) draws line segments, not a framed region
  * text styles
    * bold
    * outline, shadow
    * TODO: italic
    * TODO: underline
  * _DrawPicture
    * reads PICT v2
      * reads 1-bit pixmaps (Lemmings "special graphics" levels)
      * downsamples 8-bit images to 1-bit (Spectre Challenger)

[ams-rsrc]:  <../ams-rsrc/#readme>
