/*
	AEObjectModel.hh
	----------------
*/

#ifndef AEOBJECTMODEL_AEOBJECTMODEL_HH
#define AEOBJECTMODEL_AEOBJECTMODEL_HH

// Nitrogen
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSETOKEN_HH
#include "Mac/AppleEvents/Functions/AEDisposeToken.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYFORM_HH
#include "Mac/AppleEvents/Types/AEKeyForm.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEOBJECTCLASS_HH
#include "Mac/AppleEvents/Types/AEObjectClass.hh"
#endif


UInt32 ComputeAbsoluteIndex( const Mac::AEDesc_Data&  keyData,
                             std::size_t              count );

pascal OSErr OSLDisposeToken( AEDesc* token );

nucleus::owned< Mac::AEDesc_Token >
//
DispatchAccessToList( Mac::AEObjectClass        desiredClass,
                      const Mac::AEDesc_Token&  containerToken,
                      Mac::AEObjectClass        containerClass,
                      Mac::AEKeyForm            keyForm,
                      const Mac::AEDesc_Data&   keyData );

#endif
