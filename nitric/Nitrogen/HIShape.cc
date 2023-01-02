// Nitrogen/HIShape.cc
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

#include "Nitrogen/HIShape.hh"


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

	nucleus::owned<HIShapeRef> HIShapeCreateWithQDRgn ( RgnHandle inRgn ) {
		HIShapeRef result =       ::HIShapeCreateWithQDRgn ( inRgn );
		if ( result == NULL ) throw HIShapeCreateWithQDRgn_Failed ();
		return nucleus::owned<HIShapeRef>::seize ( result );
		}
	
	nucleus::owned<HIShapeRef> HIShapeCreateWithRect ( const HIRect &inRect ) {
		HIShapeRef result =       ::HIShapeCreateWithRect ( &inRect );
		if ( result == NULL ) throw HIShapeCreateWithRect_Failed ();
		return nucleus::owned<HIShapeRef>::seize ( result );
		}

	nucleus::owned<HIShapeRef> HIShapeCreateCopy ( HIShapeRef inShape ) {
		HIShapeRef result =       ::HIShapeCreateCopy ( inShape );
		if ( result == NULL ) throw HIShapeCreateCopy_Failed ();
		return nucleus::owned<HIShapeRef>::seize ( result );
		}

	nucleus::owned<HIShapeRef> HIShapeCreateIntersection ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateIntersection ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateIntersection_Failed ();
		return nucleus::owned<HIShapeRef>::seize ( result );
		}

	nucleus::owned<HIShapeRef> HIShapeCreateDifference ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateDifference ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateDifference_Failed ();
		return nucleus::owned<HIShapeRef>::seize ( result );
		}

	nucleus::owned<HIShapeRef> HIShapeCreateUnion ( HIShapeRef inShape1, HIShapeRef inShape2 ) {
		HIShapeRef result =       ::HIShapeCreateUnion ( inShape1, inShape2 );
		if ( result == NULL ) throw HIShapeCreateUnion_Failed ();
		return nucleus::owned<HIShapeRef>::seize ( result );
		}

#if ! __LP64__

	inline nucleus::owned<RgnHandle> HIShapeGetAsQDRgn ( HIShapeRef inShape ) {
		nucleus::owned<RgnHandle> result = nucleus::owned<RgnHandle>::seize ( NewRgn ());
		Mac::ThrowOSStatus ( ::HIShapeGetAsQDRgn ( inShape, result ));
		return result;
		}
	
#endif  // #if ! __LP64__

/*======================================================================================*/
/*  MUTABLE FUNCTIONS                                                                   */
/*======================================================================================*/

	nucleus::owned<HIMutableShapeRef> HIShapeCreateMutable ( void ) {
		HIMutableShapeRef result = ::HIShapeCreateMutable ();
		if ( result == NULL )  throw HIShapeCreateMutable_Failed ();
		return nucleus::owned<HIMutableShapeRef>::seize ( result );
		}

	nucleus::owned<HIMutableShapeRef> HIShapeCreateMutableCopy ( HIShapeRef inOrig ) {
		HIMutableShapeRef result = ::HIShapeCreateMutableCopy ( inOrig );
		if ( result == NULL )  throw HIShapeCreateMutableCopy_Failed ();
		return nucleus::owned<HIMutableShapeRef>::seize ( result );
		}

	void RegisterHIShapeErrors () {
	//	No errors at the moment -- Apple hasn't documented them
		}
	
	}

#endif
