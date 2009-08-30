// Nitrogen/HIShape.cp
// -------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifdef __MACH__

#include "Nitrogen/HIShape.h"


namespace Nitrogen {

	/*
	HIShapeErrorsRegistrationDependency::HIShapeErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	*/
	
	
	static void RegisterHIShapeErrors();
	
	
	class HIShapeErrorsRegistration
	{
		public:
			HIShapeErrorsRegistration()  { RegisterHIShapeErrors(); }
	};
	
	static HIShapeErrorsRegistration theRegistration;
	
	
/*======================================================================================*/
/*  IMMUTABLE FUNCTIONS                                                                 */
/*======================================================================================*/

	Nucleus::Owned<HIShapeRef> HIShapeCreateWithQDRgn ( RgnHandle inRgn ) {
		HIShapeRef result =       ::HIShapeCreateWithQDRgn ( inRgn );
		if ( result == NULL ) throw HIShapeCreateWithQDRgn_Failed ();
		return Nucleus::Owned<HIShapeRef>::Seize ( result );
		}
	
	Nucleus::Owned<HIShapeRef> HIShapeCreateWithRect ( const HIRect &inRect ) {
		HIShapeRef result =       ::HIShapeCreateWithRect ( &inRect );
		if ( result == NULL ) throw HIShapeCreateWithRect_Failed ();
		return Nucleus::Owned<HIShapeRef>::Seize ( result );
		}

	Nucleus::Owned<HIShapeRef> HIShapeCreateCopy ( HIShapeRef inShape ) {
		HIShapeRef result =       ::HIShapeCreateCopy ( inShape );
		if ( result == NULL ) throw HIShapeCreateCopy_Failed ();
		return Nucleus::Owned<HIShapeRef>::Seize ( result );
		}

	Nucleus::Owned<HIShapeRef> HIShapeCreateIntersection ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateIntersection ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateIntersection_Failed ();
		return Nucleus::Owned<HIShapeRef>::Seize ( result );
		}

	Nucleus::Owned<HIShapeRef> HIShapeCreateDifference ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateDifference ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateDifference_Failed ();
		return Nucleus::Owned<HIShapeRef>::Seize ( result );
		}

	Nucleus::Owned<HIShapeRef> HIShapeCreateUnion ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateUnion ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateUnion_Failed ();
		return Nucleus::Owned<HIShapeRef>::Seize ( result );
		}

	inline Nucleus::Owned<RgnHandle> HIShapeGetAsQDRgn ( HIShapeRef inShape ) {
		Nucleus::Owned<RgnHandle> result = Nucleus::Owned<RgnHandle>::Seize ( NewRgn ());
		ThrowOSStatus ( ::HIShapeGetAsQDRgn ( inShape, result ));
		return result;
		}
	
/*======================================================================================*/
/*  MUTABLE FUNCTIONS                                                                   */
/*======================================================================================*/

	Nucleus::Owned<HIMutableShapeRef> HIShapeCreateMutable ( void ) {
		HIMutableShapeRef result = ::HIShapeCreateMutable ();
		if ( result == NULL )  throw HIShapeCreateMutable_Failed ();
		return Nucleus::Owned<HIMutableShapeRef>::Seize ( result );
		}

	Nucleus::Owned<HIMutableShapeRef> HIShapeCreateMutableCopy ( HIShapeRef inOrig ) {
		HIMutableShapeRef result = ::HIShapeCreateMutableCopy ( inOrig );
		if ( result == NULL )  throw HIShapeCreateMutableCopy_Failed ();
		return Nucleus::Owned<HIMutableShapeRef>::Seize ( result );
		}

	void RegisterHIShapeErrors () {
	//	No errors at the moment -- Apple hasn't documented them
		}
	
	}

#endif

