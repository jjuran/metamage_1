//	Navigation.h
#ifndef NITROGEN_NAVIGATION_H
#define NITROGEN_NAVIGATION_H

#ifndef __NAVIGATION__
#ifdef JOSHUA_JURAN_EXPERIMENTAL
#include <Navigation.h>
#else
#include <NavigationServices/Navigation.h>
#endif
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


#if 0
typedef UInt32 NavAskSaveChangesAction;
enum {
                                        /* input action codes for NavAskSaveChanges() */
  kNavSaveChangesClosingDocument = 1,
  kNavSaveChangesQuittingApplication = 2,
  kNavSaveChangesOther          = 0
};


typedef UInt32 NavAskSaveChangesResult;
enum {
                                        /* result codes for NavAskSaveChanges() */
  kNavAskSaveChangesSave        = 1,
  kNavAskSaveChangesCancel      = 2,
  kNavAskSaveChangesDontSave    = 3
};


typedef UInt32 NavAskDiscardChangesResult;
enum {
                                        /* result codes for NavAskDiscardChanges() */
  kNavAskDiscardChanges         = 1,
  kNavAskDiscardChangesCancel   = 2
};


typedef SInt16 NavFilterModes;
enum {
                                        /* which elements are being filtered for objects: */
  kNavFilteringBrowserList      = 0,
  kNavFilteringFavorites        = 1,
  kNavFilteringRecents          = 2,
  kNavFilteringShortCutVolumes  = 3,
  kNavFilteringLocationPopup    = 4     /* for v1.1 or greater */
};


enum {
  kNavFileOrFolderVersion       = 1
};
#endif

namespace Nitrogen {

	class NavServicesErrorsRegistrationDependency
	{
		public:
			NavServicesErrorsRegistrationDependency();
	};
	
	using ::NavDialogRef;
    using ::NavReplyRecord;
  }

namespace Nucleus
  {
	template <> struct Disposer< Nitrogen::NavDialogRef >: public std::unary_function< Nitrogen::NavDialogRef, void >
		{
		void operator()( Nitrogen::NavDialogRef nav ) const
			{
				::NavDialogDispose ( nav );
			}
		};
/*
	template <> struct Disposer< Nitrogen::NavReplyRecord >: public std::unary_function< Nitrogen::NavReplyRecord, void >
		{
		void operator()( Nitrogen::NavReplyRecord &nav ) const
			{
				::NavDisposeReply ( &nav );
			}
		};
*/
  }

namespace Nitrogen
  {

#if 0
typedef UInt32 NavUserAction;

typedef SInt32 NavEventCallbackMessage;
#endif

#if 0
/*
 *  NewNavEventUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern NavEventUPP
NewNavEventUPP(NavEventProcPtr userRoutine)                   AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  NewNavPreviewUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern NavPreviewUPP
NewNavPreviewUPP(NavPreviewProcPtr userRoutine)               AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  NewNavObjectFilterUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern NavObjectFilterUPP
NewNavObjectFilterUPP(NavObjectFilterProcPtr userRoutine)     AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  DisposeNavEventUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
DisposeNavEventUPP(NavEventUPP userUPP)                       AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  DisposeNavPreviewUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
DisposeNavPreviewUPP(NavPreviewUPP userUPP)                   AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  DisposeNavObjectFilterUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
DisposeNavObjectFilterUPP(NavObjectFilterUPP userUPP)         AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  InvokeNavEventUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
InvokeNavEventUPP(
  NavEventCallbackMessage  callBackSelector,
  NavCBRecPtr              callBackParms,
  void *                   callBackUD,
  NavEventUPP              userUPP)                           AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  InvokeNavPreviewUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern Boolean
InvokeNavPreviewUPP(
  NavCBRecPtr    callBackParms,
  void *         callBackUD,
  NavPreviewUPP  userUPP)                                     AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  InvokeNavObjectFilterUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern Boolean
InvokeNavObjectFilterUPP(
  AEDesc *            theItem,
  void *              info,
  void *              callBackUD,
  NavFilterModes      filterMode,
  NavObjectFilterUPP  userUPP)                                AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif

#if 0
typedef SInt32 NavCustomControlMessage;

typedef UInt32 NavActionState;

typedef UInt16 NavPopupMenuItem;

typedef UInt16 NavSortKeyField;

typedef UInt16 NavSortOrder;

typedef UInt32 NavDialogOptionFlags;

typedef UInt32 NavTranslationOptions;
#endif


//	extern UInt32 NavLibraryVersion(void)
	using ::NavLibraryVersion;

/*	We don't provide Nitrogen versions of the deprecated functions:
		NavChooseFile
		NavChooseFolder
		NavChooseObject
		NavChooseVolume
		NavGetFile
		NavNewFolder
		NavAskDiscardChanges
		NavAskSaveChanges
		NavCustomAskSaveChanges
		NavPutFile
		NavGetDefaultDialogOptions
		
or the unsupported functions:
		NavLoad
		NavUnload
		NavServicesCanRun
*/


	

#if 0
/*
 *  NavTranslateFile()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in Carbon.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 */
extern OSErr 
NavTranslateFile(
  NavReplyRecord *        reply,
  NavTranslationOptions   howToTranslate)                     AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif


//	Semantics?? What should happen to the NavReplyRecord here?
	inline void NavCompleteSave ( NavReplyRecord &reply, NavTranslationOptions howToTranslate ) {
		(void) NavServicesErrorsRegistrationDependency();
		ThrowOSStatus ( ::NavCompleteSave ( &reply, howToTranslate ));
		}

	inline void NavCustomControl ( NavDialogRef dialog, NavCustomControlMessage selector, void *parms ) {
		(void) NavServicesErrorsRegistrationDependency();
		ThrowOSStatus ( ::NavCustomControl ( dialog, selector, parms ));
		}

	inline void NavCreatePreview (
			AEDesc *theObject,
			OSType        previewDataType	= OSType(),
			const void *  previewData		= NULL,
			Size          previewDataSize	= 0 ) {
		(void) NavServicesErrorsRegistrationDependency();
		ThrowOSStatus ( ::NavCreatePreview ( theObject, previewDataType, previewData, previewDataSize ));
		}
	
	inline void NavDisposeReply ( NavReplyRecord * reply ) {
		(void) NavServicesErrorsRegistrationDependency();
		ThrowOSStatus ( ::NavDisposeReply ( reply ));
		}		
	
	using ::NavServicesAvailable;

	inline NavDialogCreationOptions NavGetDefaultDialogCreationOptions(void) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogCreationOptions result;
		ThrowOSStatus ( ::NavGetDefaultDialogCreationOptions ( &result ));
		return result;
		}
	
	inline Nucleus::Owned<NavDialogRef> NavCreateGetFileDialog (
			const NavDialogCreationOptions *  inOptions 	= NULL,
			NavTypeListHandle                 inTypeList	= NULL,
			NavEventUPP                       inEventProc	= NULL,
			NavPreviewUPP                     inPreviewProc	= NULL,
			NavObjectFilterUPP                inFilterProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateGetFileDialog ( inOptions, inTypeList,
					inEventProc, inPreviewProc, inFilterProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreatePutFileDialog (
			const NavDialogCreationOptions *  inOptions, /* Can be NULL */
			OSType                            inFileType,
			OSType                            inFileCreator,
			NavEventUPP                       inEventProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreatePutFileDialog ( inOptions, inFileType, inFileCreator,
					inEventProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreateAskReviewDocumentsDialog (
			const NavDialogCreationOptions *  inOptions,
			UInt32                            inDocumentCount,
			NavEventUPP                       inEventProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateAskReviewDocumentsDialog ( inOptions, inDocumentCount,
					inEventProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreateAskSaveChangesDialog (
			const NavDialogCreationOptions *  inOptions,
			NavAskSaveChangesAction           inAction,
			NavEventUPP                       inEventProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateAskSaveChangesDialog ( inOptions, inAction,
					inEventProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreateAskDiscardChangesDialog (
			const NavDialogCreationOptions *  inOptions, /* Can be NULL */
			NavEventUPP                       inEventProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateAskDiscardChangesDialog ( inOptions,
					inEventProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreateChooseFileDialog (
			const NavDialogCreationOptions *  inOptions 	= NULL,
			NavTypeListHandle                 inTypeList	= NULL,
			NavEventUPP                       inEventProc	= NULL,
			NavPreviewUPP                     inPreviewProc	= NULL,
			NavObjectFilterUPP                inFilterProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateChooseFileDialog ( inOptions, inTypeList,
					inEventProc, inPreviewProc, inFilterProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreateChooseFolderDialog (
			const NavDialogCreationOptions *  inOptions 	= NULL,
			NavEventUPP                       inEventProc	= NULL,
			NavObjectFilterUPP                inFilterProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateChooseFolderDialog ( inOptions,
					inEventProc, inFilterProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreateChooseVolumeDialog (
			const NavDialogCreationOptions *  inOptions 	= NULL,
			NavEventUPP                       inEventProc	= NULL,
			NavObjectFilterUPP                inFilterProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateChooseVolumeDialog ( inOptions,
					inEventProc, inFilterProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreateChooseObjectDialog (
			const NavDialogCreationOptions *  inOptions 	= NULL,
			NavEventUPP                       inEventProc	= NULL,
			NavPreviewUPP                     inPreviewProc	= NULL,
			NavObjectFilterUPP                inFilterProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateChooseObjectDialog ( inOptions,
					inEventProc, inPreviewProc, inFilterProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline Nucleus::Owned<NavDialogRef> NavCreateNewFolderDialog (
			const NavDialogCreationOptions *  inOptions 	= NULL,
			NavEventUPP                       inEventProc	= NULL,
			void *                            inClientData 	= NULL ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavDialogRef result;
		ThrowOSStatus ( ::NavCreateNewFolderDialog ( inOptions,
					inEventProc, inClientData, &result ));
		return Nucleus::Owned<NavDialogRef>::Seize ( result );
		}

	inline void NavDialogRun ( NavDialogRef inDialog ) {
		(void) NavServicesErrorsRegistrationDependency();
		ThrowOSStatus ( ::NavDialogRun ( inDialog ));
		}

//	Since we don't want people calling NavDialogDispose, we're not 
//	providing a Nitrogen version

//	extern WindowRef NavDialogGetWindow(NavDialogRef inDialog);
	using ::NavDialogGetWindow;

//	extern NavUserAction NavDialogGetUserAction(NavDialogRef inDialog);
	using ::NavDialogGetUserAction;

#if 0
	inline Nucleus::Owned<NavReplyRecord> NavDialogGetReply ( NavDialogRef inDialog ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavReplyRecord result;
		ThrowOSStatus ( ::NavDialogGetReply ( inDialog, &result ));
		return Nucleus::Owned<NavReplyRecord>::Seize ( result );
		}
#endif
	inline NavReplyRecord NavDialogGetReply ( NavDialogRef inDialog ) {
		(void) NavServicesErrorsRegistrationDependency();
		NavReplyRecord result;
		ThrowOSStatus ( ::NavDialogGetReply ( inDialog, &result ));
		return result;
		}

//	extern CFStringRef NavDialogGetSaveFileName(NavDialogRef inPutFileDialog);
	using ::NavDialogGetSaveFileName;

	inline void NavDialogSetSaveFileName ( NavDialogRef inPutFileDialog, CFStringRef inFileName ) {
		(void) NavServicesErrorsRegistrationDependency();
		ThrowOSStatus ( ::NavDialogSetSaveFileName ( inPutFileDialog, inFileName ));
		}

	inline Boolean NavDialogGetSaveFileExtensionHidden ( NavDialogRef inPutFileDialog ) {
		return ::NavDialogGetSaveFileExtensionHidden ( inPutFileDialog );
		}

	inline void NavDialogSetSaveFileExtensionHidden ( NavDialogRef inPutFileDialog, Boolean inHidden ) {
		(void) NavServicesErrorsRegistrationDependency();
		ThrowOSStatus ( ::NavDialogSetSaveFileExtensionHidden ( inPutFileDialog, inHidden ));
		}

	}
	

#endif /* NITROGEN_NAVIGATION_H */

