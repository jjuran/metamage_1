#ifndef NITROGEN_HISHAPE_H
#include "Nitrogen/HIShape.h"
#endif

namespace Nitrogen {

/*======================================================================================*/
/*  IMMUTABLE FUNCTIONS                                                                 */
/*======================================================================================*/

	Owned<HIShapeRef> HIShapeCreateWithQDRgn ( RgnHandle inRgn ) {
		HIShapeRef result =       ::HIShapeCreateWithQDRgn ( inRgn );
		if ( result == NULL ) throw HIShapeCreateWithQDRgn_Failed ();
		return Owned<HIShapeRef>::Seize ( result );
		}
	
	Owned<HIShapeRef> HIShapeCreateWithRect ( const HIRect &inRect ) {
		HIShapeRef result =       ::HIShapeCreateWithRect ( &inRect );
		if ( result == NULL ) throw HIShapeCreateWithRect_Failed ();
		return Owned<HIShapeRef>::Seize ( result );
		}

	Owned<HIShapeRef> HIShapeCreateCopy ( HIShapeRef inShape ) {
		HIShapeRef result =       ::HIShapeCreateCopy ( inShape );
		if ( result == NULL ) throw HIShapeCreateCopy_Failed ();
		return Owned<HIShapeRef>::Seize ( result );
		}

	Owned<HIShapeRef> HIShapeCreateIntersection ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateIntersection ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateIntersection_Failed ();
		return Owned<HIShapeRef>::Seize ( result );
		}

	Owned<HIShapeRef> HIShapeCreateDifference ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateDifference ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateDifference_Failed ();
		return Owned<HIShapeRef>::Seize ( result );
		}

	Owned<HIShapeRef> HIShapeCreateUnion ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateUnion ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateUnion_Failed ();
		return Owned<HIShapeRef>::Seize ( result );
		}

	inline Owned<RgnHandle> HIShapeGetAsQDRgn ( HIShapeRef inShape ) {
     	OnlyOnce<RegisterHIShapeErrors>();
		Owned<RgnHandle> result = Owned<RgnHandle>::Seize ( NewRgn ());
		ThrowOSStatus ( ::HIShapeGetAsQDRgn ( inShape, result ));
		return result;
		}
	
/*======================================================================================*/
/*  MUTABLE FUNCTIONS                                                                   */
/*======================================================================================*/

	Owned<HIMutableShapeRef> HIShapeCreateMutable ( void ) {
		HIMutableShapeRef result = ::HIShapeCreateMutable ();
		if ( result == NULL )  throw HIShapeCreateMutable_Failed ();
		return Owned<HIMutableShapeRef>::Seize ( result );
		}

	Owned<HIMutableShapeRef> HIShapeCreateMutableCopy ( HIShapeRef inOrig ) {
		HIMutableShapeRef result = ::HIShapeCreateMutableCopy ( inOrig );
		if ( result == NULL )  throw HIShapeCreateMutableCopy_Failed ();
		return Owned<HIMutableShapeRef>::Seize ( result );
		}

	void RegisterHIShapeErrors () {
	//	No errors at the moment -- Apple hasn't documented them
		}
	
	}

