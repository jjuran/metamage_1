// Nitrogen/HIView.cc
// ------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifdef __MACH__

#include "Nitrogen/HIView.hh"


namespace Nitrogen {
	
	using Mac::ThrowOSStatus;
	
	
	HIViewErrorsRegistrationDependency::HIViewErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterHIViewErrors();
	
	
	class HIViewErrorsRegistration
	{
		public:
			HIViewErrorsRegistration()  { RegisterHIViewErrors(); }
	};
	
	static HIViewErrorsRegistration theRegistration;
	
	
#if ! __LP64__
	
	HIViewCreateOffscreenImage_Result HIViewCreateOffscreenImage ( HIViewRef inView, OptionBits inOptions ) {
		HIViewCreateOffscreenImage_Result retVal;
		CGImageRef	returnedCG;
		ThrowOSStatus ( ::HIViewCreateOffscreenImage ( inView, inOptions, &retVal.frame, &returnedCG ));
		retVal.image = nucleus::owned<CGImageRef>::seize ( returnedCG );
		return retVal;
		}
	
	
#if 0
//	Returns a new, converted region. Original region is untouched
	nucleus::owned<RgnHandle> HIViewConvertRegion ( RgnHandle inRgn, HIViewRef inSourceView, HIViewRef inDestView ) {
		nucleus::owned<RgnHandle> result = CopyRgn ( inRgn );
		ThrowOSStatus ( ::HIViewConvertRegion ( result, inSourceView, inDestView ));
		return result;
		}
#endif


#ifdef MAC_OS_X_VERSION_10_3

//	Multiple variations of "HISegmentedViewSetSegmentImage"
	void HISegmentedViewSetSegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
		HIViewImageContentInfo cinfo;
		cinfo.contentType	= kControlNoContent;
		cinfo.u.iconSuite	= NULL;	/* It's a union */
		ThrowOSStatus ( ::HISegmentedViewSetSegmentImage ( inSegmentedView, inSegmentIndexOneBased, &cinfo ));
		}
	
	void HISegmentedViewSetSegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, IconRef iconRef ) {
		HIViewImageContentInfo cinfo;
		cinfo.contentType	= kControlContentIconRef;
		cinfo.u.iconRef		= iconRef;	/* It's a union */
		ThrowOSStatus ( ::HISegmentedViewSetSegmentImage ( inSegmentedView, inSegmentIndexOneBased, &cinfo ));
		}
	
	void HISegmentedViewSetSegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased, CGImageRef imageRef ) {
		HIViewImageContentInfo cinfo;
		cinfo.contentType	= kControlContentCGImageRef;
		cinfo.u.imageRef	= imageRef;	/* It's a union */
		ThrowOSStatus ( ::HISegmentedViewSetSegmentImage ( inSegmentedView, inSegmentIndexOneBased, &cinfo ));
		}
	
#endif  // #ifdef MAC_OS_X_VERSION_10_3

#endif  // #if ! __LP64__
	
#if 0
	template < nucleus::owned<CGImageRef> > HISegmentedViewCopySegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
		HIViewImageContentInfo result;
		cinfo.contentType	= kControlContentCGImageRef;
		cinfo.u.imageRef	= NULL;	/* It's a union */
		ThrowOSStatus ( ::HISegmentedViewCopySegmentImage ( inSegmentedView, inSegmentIndexOneBased, &result ));
		return nucleus::owned<CGImageRef>::seize ( cinfo.u.imageRef );
		}
	
//	!!! Marshall says "Ewwww!"
//	!!! Should we have an nucleus::owned<HIViewImageContentInfo> here?
//	We should have a whole series of these, maybe templated on what kind of info we want to return
	inline HIViewImageContentInfo HISegmentedViewCopySegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
		HIViewImageContentInfo retVal;
		ThrowOSStatus ( ::HISegmentedViewCopySegmentImage ( inSegmentedView, inSegmentIndexOneBased, &retVal ));
		return retVal;
		}

	template <typename returnType, int returnTag>
	nucleus::owned<returnType> HISegmentedViewCopySegmentImage ( HIViewRef inSegmentedView, UInt32 inSegmentIndexOneBased ) {
		HIViewImageContentInfo cinfo;
		cinfo.contentType = returnTag;
		cinfo.u.iconSuite = NULL;	/* It's a union */
		ThrowOSStatus ( ::HISegmentedViewCopySegmentImage ( inSegmentedView, inSegmentIndexOneBased, &cinfo ));
		return nucleus::owned<returnType>::seize ( * ((returnType *) &(cinfo.u)));
		};
#endif
			
	void RegisterHIViewErrors () {
	//	Apple has not documented any at this time
		}

	}

#endif
