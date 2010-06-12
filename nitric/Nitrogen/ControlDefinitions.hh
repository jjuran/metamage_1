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

#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __TSMTE__
#include <TSMTE.h>
#endif

// Nitrogen
#include "Carbon/CF/Types/CFTypeRef.hh"
#include "Mac/Controls/Types/ControlPartCode.hh"
#include "Mac/Controls/Types/ControlProcID.hh"

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_CONTROLS_HH
#include "Nitrogen/Controls.hh"
#endif

namespace Nitrogen
  {
	
	using Mac::pushButProc;
	using Mac::checkBoxProc;
	using Mac::radioButProc;
	using Mac::scrollBarProc;
	using Mac::popupMenuProc;
	
	using Mac::kControlLabelPart;
	using Mac::kControlMenuPart;
	using Mac::kControlTrianglePart;
	using Mac::kControlEditTextPart;
	using Mac::kControlPicturePart;
	using Mac::kControlIconPart;
	using Mac::kControlClockPart;
	using Mac::kControlListBoxPart;
	using Mac::kControlListBoxDoubleClickPart;
	using Mac::kControlImageWellPart;
	using Mac::kControlRadioGroupPart;
	using Mac::kControlButtonPart;
	using Mac::kControlCheckBoxPart;
	using Mac::kControlRadioButtonPart;
	using Mac::kControlUpButtonPart;
	using Mac::kControlDownButtonPart;
	using Mac::kControlPageUpPart;
	using Mac::kControlPageDownPart;
	using Mac::kControlClockHourDayPart;
	using Mac::kControlClockMinuteMonthPart;
	using Mac::kControlClockSecondYearPart;
	using Mac::kControlClockAMPMPart;
	using Mac::kControlDataBrowserPart;
	using Mac::kControlDataBrowserDraggedPart;
	
	
	static const ControlDataTag kControlBevelButtonContentTag                  = ControlDataTag( ::kControlBevelButtonContentTag                  );
	static const ControlDataTag kControlBevelButtonTransformTag                = ControlDataTag( ::kControlBevelButtonTransformTag                );
	static const ControlDataTag kControlBevelButtonTextAlignTag                = ControlDataTag( ::kControlBevelButtonTextAlignTag                );
	static const ControlDataTag kControlBevelButtonTextOffsetTag               = ControlDataTag( ::kControlBevelButtonTextOffsetTag               );
	static const ControlDataTag kControlBevelButtonGraphicAlignTag             = ControlDataTag( ::kControlBevelButtonGraphicAlignTag             );
	static const ControlDataTag kControlBevelButtonGraphicOffsetTag            = ControlDataTag( ::kControlBevelButtonGraphicOffsetTag            );
	static const ControlDataTag kControlBevelButtonTextPlaceTag                = ControlDataTag( ::kControlBevelButtonTextPlaceTag                );
	static const ControlDataTag kControlBevelButtonMenuValueTag                = ControlDataTag( ::kControlBevelButtonMenuValueTag                );
	static const ControlDataTag kControlBevelButtonMenuRefTag                  = ControlDataTag( ::kControlBevelButtonMenuRefTag                  );
	static const ControlDataTag kControlBevelButtonCenterPopupGlyphTag         = ControlDataTag( ::kControlBevelButtonCenterPopupGlyphTag         );
	static const ControlDataTag kControlBevelButtonLastMenuTag                 = ControlDataTag( ::kControlBevelButtonLastMenuTag                 );
	static const ControlDataTag kControlBevelButtonMenuDelayTag                = ControlDataTag( ::kControlBevelButtonMenuDelayTag                );
	static const ControlDataTag kControlBevelButtonScaleIconTag                = ControlDataTag( ::kControlBevelButtonScaleIconTag                );
	static const ControlDataTag kControlBevelButtonOwnedMenuRefTag             = ControlDataTag( ::kControlBevelButtonOwnedMenuRefTag             );
	static const ControlDataTag kControlBevelButtonKindTag                     = ControlDataTag( ::kControlBevelButtonKindTag                     );
	
	static const ControlDataTag kControlTriangleLastValueTag                   = ControlDataTag( ::kControlTriangleLastValueTag                   );
	
	static const ControlDataTag kControlProgressBarIndeterminateTag            = ControlDataTag( ::kControlProgressBarIndeterminateTag            );
	static const ControlDataTag kControlProgressBarAnimatingTag                = ControlDataTag( ::kControlProgressBarAnimatingTag                );
	
	static const ControlDataTag kControlTabContentRectTag                      = ControlDataTag( ::kControlTabContentRectTag                      );
	static const ControlDataTag kControlTabEnabledFlagTag                      = ControlDataTag( ::kControlTabEnabledFlagTag                      );
	static const ControlDataTag kControlTabInfoTag                             = ControlDataTag( ::kControlTabInfoTag                             );
	
	static const ControlDataTag kControlGroupBoxMenuHandleTag                  = ControlDataTag( ::kControlGroupBoxMenuHandleTag                  );
	static const ControlDataTag kControlGroupBoxTitleRectTag                   = ControlDataTag( ::kControlGroupBoxTitleRectTag                   );
	
	static const ControlDataTag kControlImageWellIsDragDestinationTag          = ControlDataTag( ::kControlImageWellIsDragDestinationTag          );
	
	static const ControlDataTag kControlClockLongDateTag                       = ControlDataTag( ::kControlClockLongDateTag                       );
	
	static const ControlDataTag kControlUserItemDrawProcTag                    = ControlDataTag( ::kControlUserItemDrawProcTag                    );
	static const ControlDataTag kControlUserPaneDrawProcTag                    = ControlDataTag( ::kControlUserPaneDrawProcTag                    );
	static const ControlDataTag kControlUserPaneHitTestProcTag                 = ControlDataTag( ::kControlUserPaneHitTestProcTag                 );
	static const ControlDataTag kControlUserPaneTrackingProcTag                = ControlDataTag( ::kControlUserPaneTrackingProcTag                );
	static const ControlDataTag kControlUserPaneIdleProcTag                    = ControlDataTag( ::kControlUserPaneIdleProcTag                    );
	static const ControlDataTag kControlUserPaneKeyDownProcTag                 = ControlDataTag( ::kControlUserPaneKeyDownProcTag                 );
	static const ControlDataTag kControlUserPaneActivateProcTag                = ControlDataTag( ::kControlUserPaneActivateProcTag                );
	static const ControlDataTag kControlUserPaneFocusProcTag                   = ControlDataTag( ::kControlUserPaneFocusProcTag                   );
	static const ControlDataTag kControlUserPaneBackgroundProcTag              = ControlDataTag( ::kControlUserPaneBackgroundProcTag              );
	
	static const ControlDataTag kControlEditTextTextTag                        = ControlDataTag( ::kControlEditTextTextTag                        );
	static const ControlDataTag kControlEditTextTEHandleTag                    = ControlDataTag( ::kControlEditTextTEHandleTag                    );
	static const ControlDataTag kControlEditTextSelectionTag                   = ControlDataTag( ::kControlEditTextSelectionTag                   );
	static const ControlDataTag kControlEditTextPasswordTag                    = ControlDataTag( ::kControlEditTextPasswordTag                    );
	static const ControlDataTag kControlEditTextKeyScriptBehaviorTag           = ControlDataTag( ::kControlEditTextKeyScriptBehaviorTag           );
	static const ControlDataTag kControlEditTextLockedTag                      = ControlDataTag( ::kControlEditTextLockedTag                      );
	static const ControlDataTag kControlEditTextFixedTextTag                   = ControlDataTag( ::kControlEditTextFixedTextTag                   );
	static const ControlDataTag kControlEditTextValidationProcTag              = ControlDataTag( ::kControlEditTextValidationProcTag              );
	static const ControlDataTag kControlEditTextInlinePreUpdateProcTag         = ControlDataTag( ::kControlEditTextInlinePreUpdateProcTag         );
	static const ControlDataTag kControlEditTextInlinePostUpdateProcTag        = ControlDataTag( ::kControlEditTextInlinePostUpdateProcTag        );
	static const ControlDataTag kControlEditTextCFStringTag                    = ControlDataTag( ::kControlEditTextCFStringTag                    );
	static const ControlDataTag kControlEditTextPasswordCFStringTag            = ControlDataTag( ::kControlEditTextPasswordCFStringTag            );
	
	static const ControlDataTag kControlStaticTextCFStringTag                  = ControlDataTag( ::kControlStaticTextCFStringTag                  );
	
	static const ControlDataTag kControlStaticTextTextHeightTag                = ControlDataTag( ::kControlStaticTextTextHeightTag                );
	static const ControlDataTag kControlStaticTextTruncTag                     = ControlDataTag( ::kControlStaticTextTruncTag                     );
	
	static const ControlDataTag kControlPictureHandleTag                       = ControlDataTag( ::kControlPictureHandleTag                       );
	
	static const ControlDataTag kControlIconTransformTag                       = ControlDataTag( ::kControlIconTransformTag                       );
	static const ControlDataTag kControlIconAlignmentTag                       = ControlDataTag( ::kControlIconAlignmentTag                       );
	static const ControlDataTag kControlIconResourceIDTag                      = ControlDataTag( ::kControlIconResourceIDTag                      );
	
	static const ControlDataTag kControlListBoxListHandleTag                   = ControlDataTag( ::kControlListBoxListHandleTag                   );
	static const ControlDataTag kControlListBoxDoubleClickTag                  = ControlDataTag( ::kControlListBoxDoubleClickTag                  );
	static const ControlDataTag kControlListBoxLDEFTag                         = ControlDataTag( ::kControlListBoxLDEFTag                         );
	
	static const ControlDataTag kControlPushButtonDefaultTag                   = ControlDataTag( ::kControlPushButtonDefaultTag                   );
	static const ControlDataTag kControlPushButtonCancelTag                    = ControlDataTag( ::kControlPushButtonCancelTag                    );
	
	static const ControlDataTag kControlPopupButtonMenuIDTag                   = ControlDataTag( ::kControlPopupButtonMenuIDTag                   );
	static const ControlDataTag kControlPopupButtonExtraHeightTag              = ControlDataTag( ::kControlPopupButtonExtraHeightTag              );
	static const ControlDataTag kControlPopupButtonCheckCurrentTag             = ControlDataTag( ::kControlPopupButtonCheckCurrentTag             );
	
	static const ControlDataTag kControlScrollTextBoxDelayBeforeAutoScrollTag  = ControlDataTag( ::kControlScrollTextBoxDelayBeforeAutoScrollTag  );
	static const ControlDataTag kControlScrollTextBoxDelayBetweenAutoScrollTag = ControlDataTag( ::kControlScrollTextBoxDelayBetweenAutoScrollTag );
	static const ControlDataTag kControlScrollTextBoxAutoScrollAmountTag       = ControlDataTag( ::kControlScrollTextBoxAutoScrollAmountTag       );
	static const ControlDataTag kControlScrollTextBoxContentsTag               = ControlDataTag( ::kControlScrollTextBoxContentsTag               );
	
	static const ControlDataTag kControlEditUnicodeTextPostUpdateProcTag       = ControlDataTag( ::kControlEditUnicodeTextPostUpdateProcTag       );
	
	
   template<> struct ControlData_Traits< kControlBevelButtonContentTag                  >: nucleus::POD_scribe    < ControlButtonContentInfo      > {};
   template<> struct ControlData_Traits< kControlBevelButtonTransformTag                >: nucleus::POD_scribe    < IconTransformType             > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextAlignTag                >: nucleus::POD_scribe    < ControlButtonTextAlignment    > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextOffsetTag               >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonGraphicAlignTag             >: nucleus::POD_scribe    < ControlButtonGraphicAlignment > {};
   template<> struct ControlData_Traits< kControlBevelButtonGraphicOffsetTag            >: nucleus::POD_scribe    < Point                         > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextPlaceTag                >: nucleus::POD_scribe    < ControlButtonTextPlacement    > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuValueTag                >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuRefTag                  >: nucleus::POD_scribe    < MenuRef                       > {};
   template<> struct ControlData_Traits< kControlBevelButtonCenterPopupGlyphTag         >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlBevelButtonLastMenuTag                 >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuDelayTag                >: nucleus::POD_scribe    < ::SInt32                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonScaleIconTag                >: BooleanFlattener                                           {};
//   template<> struct ControlData_Traits< kControlBevelButtonOwnedMenuRefTag             >: nucleus::POD_scribe    < nucleus::owned<MenuRef>                > {};
   template<> struct ControlData_Traits< kControlBevelButtonKindTag                     >: nucleus::POD_scribe    < ThemeButtonKind               > {};

   template<> struct ControlData_Traits< kControlTriangleLastValueTag                   >: nucleus::POD_scribe    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlProgressBarIndeterminateTag            >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlProgressBarAnimatingTag                >: BooleanFlattener                                           {};

   template<> struct ControlData_Traits< kControlTabContentRectTag                      >: nucleus::POD_scribe    < Rect                          > {};
   template<> struct ControlData_Traits< kControlTabEnabledFlagTag                      >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlTabInfoTag                             >: nucleus::POD_scribe    < ControlTabInfoRec             > {};

   template<> struct ControlData_Traits< kControlGroupBoxMenuHandleTag                  >: nucleus::POD_scribe    < MenuRef                       > {};
   template<> struct ControlData_Traits< kControlGroupBoxTitleRectTag                   >: nucleus::POD_scribe    < Rect                          > {};

   template<> struct ControlData_Traits< kControlImageWellIsDragDestinationTag          >: BooleanFlattener                                           {};

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

   template<> struct ControlData_Traits< kControlEditTextTextTag                        >: DescType_Traits          < typeChar                      > {};
   template<> struct ControlData_Traits< kControlEditTextTEHandleTag                    >: nucleus::POD_scribe    < TEHandle                      > {};
   template<> struct ControlData_Traits< kControlEditTextSelectionTag                   >: nucleus::POD_scribe    < ControlEditTextSelectionRec   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordTag                    >: DescType_Traits          < typeChar                      > {};
   template<> struct ControlData_Traits< kControlEditTextKeyScriptBehaviorTag           >: nucleus::POD_scribe    < ControlKeyScriptBehavior      > {};
   template<> struct ControlData_Traits< kControlEditTextLockedTag                      >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlEditTextFixedTextTag                   >: DescType_Traits          < typeChar                      > {};
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
   template<> struct ControlData_Traits< kControlListBoxDoubleClickTag                  >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlListBoxLDEFTag                         >: nucleus::POD_scribe    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlPushButtonDefaultTag                   >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlPushButtonCancelTag                    >: BooleanFlattener                                           {};

   template<> struct ControlData_Traits< kControlPopupButtonMenuIDTag                   >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlPopupButtonExtraHeightTag              >: nucleus::POD_scribe    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlPopupButtonCheckCurrentTag             >: BooleanFlattener                                           {};

   template<> struct ControlData_Traits< kControlScrollTextBoxDelayBeforeAutoScrollTag  >: nucleus::POD_scribe    < ::UInt32                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxDelayBetweenAutoScrollTag >: nucleus::POD_scribe    < ::UInt32                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxAutoScrollAmountTag       >: nucleus::POD_scribe    < ::UInt16                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxContentsTag               >: nucleus::POD_scribe    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlEditUnicodeTextPostUpdateProcTag       >: nucleus::POD_scribe    < EditUnicodePostUpdateUPP      > {};
	
	using Mac::kControlScrollBarProc;
	using Mac::kControlScrollBarLiveProc;
	
  }

#endif
