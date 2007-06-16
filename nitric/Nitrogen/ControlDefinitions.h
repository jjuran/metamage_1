// Nitrogen/ControlDefinitions.h
// -----------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CONTROLDEFINITIONS_H
#define NITROGEN_CONTROLDEFINITIONS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CONTROLDEFINITIONS__
#include FRAMEWORK_HEADER(HIToolbox,ControlDefinitions.h)
#endif
#ifndef NITROGEN_TSMTE_H
#include "Nitrogen/TSMTE.h"
#endif
#ifndef NITROGEN_CONTROLS_H
#include "Nitrogen/Controls.h"
#endif

namespace Nitrogen
  {
	
	static const ControlProcID pushButProc   = ControlProcID( ::pushButProc   );
	static const ControlProcID checkBoxProc  = ControlProcID( ::checkBoxProc  );
	static const ControlProcID radioButProc  = ControlProcID( ::radioButProc  );
	static const ControlProcID scrollBarProc = ControlProcID( ::scrollBarProc );
	static const ControlProcID popupMenuProc = ControlProcID( ::popupMenuProc );
	
	static const ControlPartCode kControlLabelPart              = ControlPartCode( ::kControlLabelPart              );
	static const ControlPartCode kControlMenuPart               = ControlPartCode( ::kControlMenuPart               );
	static const ControlPartCode kControlTrianglePart           = ControlPartCode( ::kControlTrianglePart           );
	static const ControlPartCode kControlEditTextPart           = ControlPartCode( ::kControlEditTextPart           );
	static const ControlPartCode kControlPicturePart            = ControlPartCode( ::kControlPicturePart            );
	static const ControlPartCode kControlIconPart               = ControlPartCode( ::kControlIconPart               );
	static const ControlPartCode kControlClockPart              = ControlPartCode( ::kControlClockPart              );
	static const ControlPartCode kControlListBoxPart            = ControlPartCode( ::kControlListBoxPart            );
	static const ControlPartCode kControlListBoxDoubleClickPart = ControlPartCode( ::kControlListBoxDoubleClickPart );
	static const ControlPartCode kControlImageWellPart          = ControlPartCode( ::kControlImageWellPart          );
	static const ControlPartCode kControlRadioGroupPart         = ControlPartCode( ::kControlRadioGroupPart         );
	static const ControlPartCode kControlButtonPart             = ControlPartCode( ::kControlButtonPart             );
	static const ControlPartCode kControlCheckBoxPart           = ControlPartCode( ::kControlCheckBoxPart           );
	static const ControlPartCode kControlRadioButtonPart        = ControlPartCode( ::kControlRadioButtonPart        );
	static const ControlPartCode kControlUpButtonPart           = ControlPartCode( ::kControlUpButtonPart           );
	static const ControlPartCode kControlDownButtonPart         = ControlPartCode( ::kControlDownButtonPart         );
	static const ControlPartCode kControlPageUpPart             = ControlPartCode( ::kControlPageUpPart             );
	static const ControlPartCode kControlPageDownPart           = ControlPartCode( ::kControlPageDownPart           );
	static const ControlPartCode kControlClockHourDayPart       = ControlPartCode( ::kControlClockHourDayPart       );
	static const ControlPartCode kControlClockMinuteMonthPart   = ControlPartCode( ::kControlClockMinuteMonthPart   );
	static const ControlPartCode kControlClockSecondYearPart    = ControlPartCode( ::kControlClockSecondYearPart    );
	static const ControlPartCode kControlClockAMPMPart          = ControlPartCode( ::kControlClockAMPMPart          );
	static const ControlPartCode kControlDataBrowserPart        = ControlPartCode( ::kControlDataBrowserPart        );
	static const ControlPartCode kControlDataBrowserDraggedPart = ControlPartCode( ::kControlDataBrowserDraggedPart );
	
	
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
	
	
   template<> struct ControlData_Traits< kControlBevelButtonContentTag                  >: Nucleus::PodFlattener    < ControlButtonContentInfo      > {};
   template<> struct ControlData_Traits< kControlBevelButtonTransformTag                >: Nucleus::PodFlattener    < IconTransformType             > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextAlignTag                >: Nucleus::PodFlattener    < ControlButtonTextAlignment    > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextOffsetTag               >: Nucleus::PodFlattener    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonGraphicAlignTag             >: Nucleus::PodFlattener    < ControlButtonGraphicAlignment > {};
   template<> struct ControlData_Traits< kControlBevelButtonGraphicOffsetTag            >: Nucleus::PodFlattener    < Point                         > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextPlaceTag                >: Nucleus::PodFlattener    < ControlButtonTextPlacement    > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuValueTag                >: Nucleus::PodFlattener    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuRefTag                  >: Nucleus::PodFlattener    < MenuRef                       > {};
   template<> struct ControlData_Traits< kControlBevelButtonCenterPopupGlyphTag         >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlBevelButtonLastMenuTag                 >: Nucleus::PodFlattener    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuDelayTag                >: Nucleus::PodFlattener    < ::SInt32                      > {};
   template<> struct ControlData_Traits< kControlBevelButtonScaleIconTag                >: BooleanFlattener                                           {};
//   template<> struct ControlData_Traits< kControlBevelButtonOwnedMenuRefTag             >: Nucleus::PodFlattener    < Nucleus::Owned<MenuRef>                > {};
   template<> struct ControlData_Traits< kControlBevelButtonKindTag                     >: Nucleus::PodFlattener    < ThemeButtonKind               > {};

   template<> struct ControlData_Traits< kControlTriangleLastValueTag                   >: Nucleus::PodFlattener    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlProgressBarIndeterminateTag            >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlProgressBarAnimatingTag                >: BooleanFlattener                                           {};

   template<> struct ControlData_Traits< kControlTabContentRectTag                      >: Nucleus::PodFlattener    < Rect                          > {};
   template<> struct ControlData_Traits< kControlTabEnabledFlagTag                      >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlTabInfoTag                             >: Nucleus::PodFlattener    < ControlTabInfoRec             > {};

   template<> struct ControlData_Traits< kControlGroupBoxMenuHandleTag                  >: Nucleus::PodFlattener    < MenuRef                       > {};
   template<> struct ControlData_Traits< kControlGroupBoxTitleRectTag                   >: Nucleus::PodFlattener    < Rect                          > {};

   template<> struct ControlData_Traits< kControlImageWellIsDragDestinationTag          >: BooleanFlattener                                           {};

   template<> struct ControlData_Traits< kControlClockLongDateTag                       >: Nucleus::PodFlattener    < LongDateRec                   > {};

   template<> struct ControlData_Traits< kControlUserItemDrawProcTag                    >: Nucleus::PodFlattener    < UserItemUPP                   > {};
   template<> struct ControlData_Traits< kControlUserPaneDrawProcTag                    >: Nucleus::PodFlattener    < ControlUserPaneDrawUPP        > {};
   template<> struct ControlData_Traits< kControlUserPaneHitTestProcTag                 >: Nucleus::PodFlattener    < ControlUserPaneHitTestUPP     > {};
   template<> struct ControlData_Traits< kControlUserPaneTrackingProcTag                >: Nucleus::PodFlattener    < ControlUserPaneTrackingUPP    > {};
   template<> struct ControlData_Traits< kControlUserPaneIdleProcTag                    >: Nucleus::PodFlattener    < ControlUserPaneIdleUPP        > {};
   template<> struct ControlData_Traits< kControlUserPaneKeyDownProcTag                 >: Nucleus::PodFlattener    < ControlUserPaneKeyDownUPP     > {};
   template<> struct ControlData_Traits< kControlUserPaneActivateProcTag                >: Nucleus::PodFlattener    < ControlUserPaneActivateUPP    > {};
   template<> struct ControlData_Traits< kControlUserPaneFocusProcTag                   >: Nucleus::PodFlattener    < ControlUserPaneFocusUPP       > {};
   template<> struct ControlData_Traits< kControlUserPaneBackgroundProcTag              >: Nucleus::PodFlattener    < ControlUserPaneBackgroundUPP  > {};

   template<> struct ControlData_Traits< kControlEditTextTextTag                        >: DescType_Traits          < typeChar                      > {};
   template<> struct ControlData_Traits< kControlEditTextTEHandleTag                    >: Nucleus::PodFlattener    < TEHandle                      > {};
   template<> struct ControlData_Traits< kControlEditTextSelectionTag                   >: Nucleus::PodFlattener    < ControlEditTextSelectionRec   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordTag                    >: DescType_Traits          < typeChar                      > {};
   template<> struct ControlData_Traits< kControlEditTextKeyScriptBehaviorTag           >: Nucleus::PodFlattener    < ControlKeyScriptBehavior      > {};
   template<> struct ControlData_Traits< kControlEditTextLockedTag                      >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlEditTextFixedTextTag                   >: DescType_Traits          < typeChar                      > {};
   template<> struct ControlData_Traits< kControlEditTextValidationProcTag              >: Nucleus::PodFlattener    < ControlEditTextValidationUPP  > {};
   template<> struct ControlData_Traits< kControlEditTextInlinePreUpdateProcTag         >: Nucleus::PodFlattener    < TSMTEPreUpdateUPP             > {};
   template<> struct ControlData_Traits< kControlEditTextInlinePostUpdateProcTag        >: Nucleus::PodFlattener    < TSMTEPostUpdateUPP            > {};
   template<> struct ControlData_Traits< kControlEditTextCFStringTag                    >: Nucleus::SeizingPODFlattener< CFStringRef                   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordCFStringTag            >: Nucleus::SeizingPODFlattener< CFStringRef                   > {};
   
   // Same as kControlEditTextCFStringTag
   //template<> struct ControlData_Traits< kControlStaticTextCFStringTag                  >: Nucleus::SeizingPODFlattener< CFStringRef                   > {};

   template<> struct ControlData_Traits< kControlStaticTextTextHeightTag                >: Nucleus::PodFlattener    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlStaticTextTruncTag                     >: Nucleus::PodFlattener    < TruncCode                     > {};

   template<> struct ControlData_Traits< kControlPictureHandleTag                       >: Nucleus::PodFlattener    < PicHandle                     > {};

   template<> struct ControlData_Traits< kControlIconTransformTag                       >: Nucleus::PodFlattener    < IconTransformType             > {};
   template<> struct ControlData_Traits< kControlIconAlignmentTag                       >: Nucleus::PodFlattener    < IconAlignmentType             > {};
   template<> struct ControlData_Traits< kControlIconResourceIDTag                      >: Nucleus::PodFlattener    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlListBoxListHandleTag                   >: Nucleus::PodFlattener    < ListHandle                    > {};
   template<> struct ControlData_Traits< kControlListBoxDoubleClickTag                  >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlListBoxLDEFTag                         >: Nucleus::PodFlattener    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlPushButtonDefaultTag                   >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlPushButtonCancelTag                    >: BooleanFlattener                                           {};

   template<> struct ControlData_Traits< kControlPopupButtonMenuIDTag                   >: Nucleus::PodFlattener    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlPopupButtonExtraHeightTag              >: Nucleus::PodFlattener    < ::SInt16                      > {};
   template<> struct ControlData_Traits< kControlPopupButtonCheckCurrentTag             >: BooleanFlattener                                           {};

   template<> struct ControlData_Traits< kControlScrollTextBoxDelayBeforeAutoScrollTag  >: Nucleus::PodFlattener    < ::UInt32                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxDelayBetweenAutoScrollTag >: Nucleus::PodFlattener    < ::UInt32                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxAutoScrollAmountTag       >: Nucleus::PodFlattener    < ::UInt16                      > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxContentsTag               >: Nucleus::PodFlattener    < ::SInt16                      > {};

   template<> struct ControlData_Traits< kControlEditUnicodeTextPostUpdateProcTag       >: Nucleus::PodFlattener    < EditUnicodePostUpdateUPP      > {};
	
	
	static const ControlProcID kControlScrollBarProc     = ControlProcID( ::kControlScrollBarProc     );
	static const ControlProcID kControlScrollBarLiveProc = ControlProcID( ::kControlScrollBarLiveProc );
	
  }

#endif
