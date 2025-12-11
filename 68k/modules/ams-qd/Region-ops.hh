/*
	Region-ops.hh
	-------------
*/

#ifndef REGIONOPS_HH
#define REGIONOPS_HH


struct MacRegion;

typedef MacRegion** RgnHandle;


pascal void OffsetRgn_patch( RgnHandle rgn, short dh, short dv );
pascal void InsetRgn_patch ( RgnHandle rgn, short dh, short dv );

pascal void SectRgn_patch ( RgnHandle a, RgnHandle b, RgnHandle dst );
pascal void UnionRgn_patch( RgnHandle a, RgnHandle b, RgnHandle dst );
pascal void DiffRgn_patch ( RgnHandle a, RgnHandle b, RgnHandle dst );
pascal void XOrRgn_patch  ( RgnHandle a, RgnHandle b, RgnHandle dst );

#endif
