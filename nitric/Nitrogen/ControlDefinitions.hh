// Nitrogen/ControlDefinitions.hh
// ------------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CONTROLDEFINITIONS_HH
#define NITROGEN_CONTROLDEFINITIONS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __TSMTE__
#include <TSMTE.h>
#endif

// nucleus
#ifndef NUCLEUS_SEIZINGPODSCRIBE_HH
#include "nucleus/seizing_POD_scribe.hh"
#endif
#ifndef NUCLEUS_STRINGSCRIBE_HH
#include "nucleus/string_scribe.hh"
#endif

// Nitrogen
#ifndef CARBON_CF_TYPES_CFTYPEREF_HH
#include "Carbon/CF/Types/CFTypeRef.hh"
#endif
#ifndef MAC_CONTROLS_TYPES_CONTROLPARTCODE_HH
#include "Mac/Controls/Types/ControlPartCode.hh"
#endif
#ifndef MAC_CONTROLS_TYPES_CONTROLPROCID_HH
#include "Mac/Controls/Types/ControlProcID.hh"
#endif
#ifndef MAC_TOOLBOX_TYPES_BOOLEAN_HH
#include "Mac/Toolbox/Types/Boolean.hh"
#endif

#ifndef NITROGEN_CONTROLS_HH
#include "Nitrogen/Controls.hh"
#endif


namespace Nitrogen
  {
	
   template<> struct ControlData_Traits< kControlBevelButtonContentTag                  >: nucleus::POD_scribe    < ControlButtonContentInfo      > {};
   template<> struct ControlData_Traits< kControlBevelButtonTransformTag                >: nucleus::POD_scribe    < IconTransformType             > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextAlignTag                >: nucleus::POD_scribe    < ControlButtonTextAlignment    > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextOffsetTag               >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonGraphicAlignTag             >: nucleus::POD_scribe    < ControlButtonGraphicAlignment > {};
   template<> struct ControlData_Traits< kControlBevelButtonGraphicOffsetTag            >: nucleus::POD_scribe    < Point                         > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextPlaceTag                >: nucleus::POD_scribe    < ControlButtonTextPlacement    > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuValueTag                >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuRefTag                  >: nucleus::POD_scribe    < MenuRef                       > {};
   template<> struct ControlData_Traits< kControlBevelButtonCenterPopupGlyphTag         >: Mac::Boolean_scribe                                      {};
   template<> struct ControlData_Traits< kControlBevelButtonLastMenuTag                 >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuDelayTag                >: nucleus::POD_scribe    < ::SInt32                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonScaleIconTag                >: Mac::Boolean_scribe                                      {};
//   template<> struct ControlData_Traits< kControlBevelButtonOwnedMenuRefTag             >: nucleus::POD_scribe    < nucleus::owned<MenuRef>                > {};
   template<> struct ControlData_Traits< kControlBevelButtonKindTag                     >: nucleus::POD_scribe    < ThemeButtonKind               > {};

   template<> struct ControlData_Traits< kControlTriangleLastValueTag                   >: nucleus::POD_scribe    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlProgressBarIndeterminateTag            >: Mac::Boolean_scribe                                      {};
   template<> struct ControlData_Traits< kControlProgressBarAnimatingTag                >: Mac::Boolean_scribe                                      {};

   template<> struct ControlData_Traits< kControlTabContentRectTag                      >: nucleus::POD_scribe    < Rect                          > {};
   template<> struct ControlData_Traits< kControlTabEnabledFlagTag                      >: Mac::Boolean_scribe                                      {};
   template<> struct ControlData_Traits< kControlTabInfoTag                             >: nucleus::POD_scribe    < ControlTabInfoRec             > {};

   template<> struct ControlData_Traits< kControlGroupBoxMenuHandleTag                  >: nucleus::POD_scribe    < MenuRef                       > {};
   template<> struct ControlData_Traits< kControlGroupBoxTitleRectTag                   >: nucleus::POD_scribe    < Rect                          > {};

   template<> struct ControlData_Traits< kControlImageWellIsDragDestinationTag          >: Mac::Boolean_scribe                                      {};

   template<> struct ControlData_Traits< kControlClockLongDateTag                       >: nucleus::POD_scribe    < LongDateRec                   > {};

   template<> struct ControlData_Traits< kControlUserItemDrawProcTag                    >: nucleus::POD_scribe    < UserItemUPP                   > {};
   template<> struct ControlData_Traits< kControlUserPaneDrawProcTag                    >: nucleus::POD_scribe    < ControlUserPaneDrawUPP        > {};
   template<> struct ControlData_Traits< kControlUserPaneHitTestProcTag                 >: nucleus::POD_scribe    < ControlUserPaneHitTestUPP     > {};
   template<> struct ControlData_Traits< kControlUserPaneTrackingProcTag                >: nucleus::POD_scribe    < ControlUserPaneTrackingUPP    > {};
   template<> struct ControlData_Traits< kControlUserPaneIdleProcTag                    >: nucleus::POD_scribe    < ControlUserPaneIdleUPP        > {};
   template<> struct ControlData_Traits< kControlUserPaneKeyDownProcTag                 >: nucleus::POD_scribe    < ControlUserPaneKeyDownUPP     > {};
   template<> struct ControlData_Traits< kControlUserPaneActivateProcTag                >: nucleus::POD_scribe    < ControlUserPaneActivateUPP    > {};
   template<> struct ControlData_Traits< kControlUserPaneFocusProcTag                   >: nucleus::POD_scribe    < ControlUserPaneFocusUPP       > {};
   template<> struct ControlData_Traits< kControlUserPaneBackgroundProcTag              >: nucleus::POD_scribe    < ControlUserPaneBackgroundUPP  > {};

   template<> struct ControlData_Traits< kControlEditTextTextTag                        >: nucleus::string_scribe                                   {};
   template<> struct ControlData_Traits< kControlEditTextTEHandleTag                    >: nucleus::POD_scribe    < TEHandle                      > {};
   template<> struct ControlData_Traits< kControlEditTextSelectionTag                   >: nucleus::POD_scribe    < ControlEditTextSelectionRec   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordTag                    >: nucleus::string_scribe                                   {};
   template<> struct ControlData_Traits< kControlEditTextKeyScriptBehaviorTag           >: nucleus::POD_scribe    < ControlKeyScriptBehavior      > {};
   template<> struct ControlData_Traits< kControlEditTextLockedTag                      >: Mac::Boolean_scribe                                      {};
   template<> struct ControlData_Traits< kControlEditTextFixedTextTag                   >: nucleus::string_scribe                                   {};
   template<> struct ControlData_Traits< kControlEditTextValidationProcTag              >: nucleus::POD_scribe    < ControlEditTextValidationUPP  > {};
   template<> struct ControlData_Traits< kControlEditTextInlinePreUpdateProcTag         >: nucleus::POD_scribe    < TSMTEPreUpdateUPP             > {};
   template<> struct ControlData_Traits< kControlEditTextInlinePostUpdateProcTag        >: nucleus::POD_scribe    < TSMTEPostUpdateUPP            > {};
   template<> struct ControlData_Traits< kControlEditTextCFStringTag                    >: nucleus::seizing_POD_scribe< CFStringRef                   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordCFStringTag            >: nucleus::seizing_POD_scribe< CFStringRef                   > {};
   
   // Same as kControlEditTextCFStringTag
   //template<> struct ControlData_Traits< kControlStaticTextCFStringTag                  >: nucleus::seizing_POD_scribe< CFStringRef                   > {};

   template<> struct ControlData_Traits< kControlStaticTextTextHeightTag                >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlStaticTextTruncTag                     >: nucleus::POD_scribe    < TruncCode                     > {};

   template<> struct ControlData_Traits< kControlPictureHandleTag                       >: nucleus::POD_scribe    < PicHandle                     > {};

   template<> struct ControlData_Traits< kControlIconTransformTag                       >: nucleus::POD_scribe    < IconTransformType             > {};
   template<> struct ControlData_Traits< kControlIconAlignmentTag                       >: nucleus::POD_scribe    < IconAlignmentType             > {};
   template<> struct ControlData_Traits< kControlIconResourceIDTag                      >: nucleus::POD_scribe    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlListBoxListHandleTag                   >: nucleus::POD_scribe    < ListHandle                    > {};
   template<> struct ControlData_Traits< kControlListBoxDoubleClickTag                  >: Mac::Boolean_scribe                                      {};
   template<> struct ControlData_Traits< kControlListBoxLDEFTag                         >: nucleus::POD_scribe    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlPushButtonDefaultTag                   >: Mac::Boolean_scribe                                      {};
   template<> struct ControlData_Traits< kControlPushButtonCancelTag                    >: Mac::Boolean_scribe                                      {};

   template<> struct ControlData_Traits< kControlPopupButtonMenuIDTag                   >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlPopupButtonExtraHeightTag              >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlPopupButtonCheckCurrentTag             >: Mac::Boolean_scribe                                      {};

   template<> struct ControlData_Traits< kControlScrollTextBoxDelayBeforeAutoScrollTag  >: nucleus::POD_scribe    < ::UInt32                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxDelayBetweenAutoScrollTag >: nucleus::POD_scribe    < ::UInt32                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxAutoScrollAmountTag       >: nucleus::POD_scribe    < ::UInt16                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxContentsTag               >: nucleus::POD_scribe    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlEditUnicodeTextPostUpdateProcTag       >: nucleus::POD_scribe    < EditUnicodePostUpdateUPP      > {};
	
  }

#endif
