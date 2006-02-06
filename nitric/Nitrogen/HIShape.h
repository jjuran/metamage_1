#ifndef NITROGEN_HISHAPE_H
#define NITROGEN_HISHAPE_H

#if !TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __HISHAPE__
#include <HIToolbox/HIShape.h>
#endif

#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen {

	void RegisterHIShapeErrors();

	using ::HIShapeRef;
	template <> struct OwnedDefaults< HIShapeRef >: OwnedDefaults< CFTypeRef >  {};
	template <> struct CFType_Traits< HIShapeRef >: Basic_CFType_Traits< HIShapeRef, ::HIShapeGetTypeID > {};

	using ::HIMutableShapeRef;
	template <> struct OwnedDefaults< HIMutableShapeRef >: OwnedDefaults< CFTypeRef >  {};
	template <> struct CFType_Traits< HIMutableShapeRef >: Basic_CFType_Traits< HIMutableShapeRef, ::HIShapeGetTypeID > {};

//	extern CFTypeID HIShapeGetTypeID(void)
	using ::HIShapeGetTypeID;

/*======================================================================================*/
/*  IMMUTABLE FUNCTIONS                                                                 */
/*======================================================================================*/

	class HIShapeCreateWithQDRgn_Failed {};
	Owned<HIShapeRef> HIShapeCreateWithQDRgn ( RgnHandle inRgn );

	class HIShapeCreateWithRect_Failed {};
	Owned<HIShapeRef> HIShapeCreateWithRect ( const HIRect &inRect );

	class HIShapeCreateCopy_Failed {};
	Owned<HIShapeRef> HIShapeCreateCopy ( HIShapeRef inShape );

	class HIShapeCreateIntersection_Failed {};
	Owned<HIShapeRef> HIShapeCreateIntersection ( HIShapeRef inShape1, HIShapeRef inShape2 );

	class HIShapeCreateDifference_Failed {};
	Owned<HIShapeRef> HIShapeCreateDifference ( HIShapeRef inShape1, HIShapeRef inShape2 );

	class HIShapeCreateUnion_Failed {};
	Owned<HIShapeRef> HIShapeCreateUnion ( HIShapeRef inShape1, HIShapeRef inShape2 );

//	extern Boolean HIShapeIsEmpty(HIShapeRef inShape)
	using ::HIShapeIsEmpty;
	
//	extern Boolean HIShapeIsRectangular(HIShapeRef inShape)
	using ::HIShapeIsRectangular;

	inline Boolean HIShapeContainsPoint ( HIShapeRef inShape, const HIPoint &inPoint ) {
		return ::HIShapeContainsPoint ( inShape, &inPoint );
		}
	
	inline HIRect HIShapeGetBounds ( HIShapeRef inShape ) {
		HIRect result;
		(void) ::HIShapeGetBounds ( inShape, &result );
		return result;
		}
	
	inline Owned<RgnHandle> HIShapeGetAsQDRgn ( HIShapeRef inShape );
	
	inline void HIShapeReplacePathInCGContext ( HIShapeRef inShape, CGContextRef inContext ) {
     	OnlyOnce<RegisterHIShapeErrors>();
		ThrowOSStatus ( ::HIShapeReplacePathInCGContext ( inShape, inContext ));
		}

	inline void HIShapeSetQDClip ( HIShapeRef inShape, CGrafPtr inPort ) {
     	OnlyOnce<RegisterHIShapeErrors>();
		ThrowOSStatus ( ::HIShapeSetQDClip ( inShape, inPort ));
		}

/*======================================================================================*/
/*  MUTABLE FUNCTIONS                                                                   */
/*======================================================================================*/

	class HIShapeCreateMutable_Failed {};
	Owned<HIMutableShapeRef> HIShapeCreateMutable ( void );

	class HIShapeCreateMutableCopy_Failed {};
	Owned<HIMutableShapeRef> HIShapeCreateMutableCopy ( HIShapeRef inOrig );

	inline void HIShapeSetEmpty ( HIMutableShapeRef inShape ) {
     	OnlyOnce<RegisterHIShapeErrors>();
		ThrowOSStatus ( ::HIShapeSetEmpty ( inShape ));
		}
	
	inline void HIShapeIntersect ( HIShapeRef inShape1, HIShapeRef inShape2, HIMutableShapeRef outResult ) {
     	OnlyOnce<RegisterHIShapeErrors>();
		ThrowOSStatus ( ::HIShapeIntersect ( inShape1, inShape2, outResult ));
		}

	inline void HIShapeDifference ( HIShapeRef inShape1, HIShapeRef inShape2, HIMutableShapeRef outResult ) {
     	OnlyOnce<RegisterHIShapeErrors>();
		ThrowOSStatus ( ::HIShapeDifference ( inShape1, inShape2, outResult ));
		}

	inline void HIShapeUnion ( HIShapeRef inShape1, HIShapeRef inShape2, HIMutableShapeRef outResult ) {
     	OnlyOnce<RegisterHIShapeErrors>();
		ThrowOSStatus ( ::HIShapeUnion ( inShape1, inShape2, outResult ));
		}

	inline void HIShapeOffset ( HIMutableShapeRef inShape, float inDX, float inDY ) {
     	OnlyOnce<RegisterHIShapeErrors>();
		ThrowOSStatus ( ::HIShapeOffset ( inShape, inDX, inDY ));
		}

	}


#endif /* NITROGEN_HISHAPE_H */

