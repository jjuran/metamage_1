// ControlDefinitions.h

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
	
	inline ControlProcID PushButProc  ()  { return ControlProcID::Make( pushButProc   ); }
	inline ControlProcID CheckBoxProc ()  { return ControlProcID::Make( checkBoxProc  ); }
	inline ControlProcID RadioButProc ()  { return ControlProcID::Make( radioButProc  ); }
	inline ControlProcID ScrollBarProc()  { return ControlProcID::Make( scrollBarProc ); }
	inline ControlProcID PopupMenuProc()  { return ControlProcID::Make( popupMenuProc ); }
	
	inline ControlPartCode ControlButtonPart     ()  { return ControlPartCode::Make( kControlButtonPart      ); }
	inline ControlPartCode ControlCheckBoxPart   ()  { return ControlPartCode::Make( kControlCheckBoxPart    ); }
	inline ControlPartCode ControlRadioButtonPart()  { return ControlPartCode::Make( kControlRadioButtonPart ); }
	inline ControlPartCode ControlUpButtonPart   ()  { return ControlPartCode::Make( kControlUpButtonPart    ); }
	inline ControlPartCode ControlDownButtonPart ()  { return ControlPartCode::Make( kControlDownButtonPart  ); }
	inline ControlPartCode ControlPageUpPart     ()  { return ControlPartCode::Make( kControlPageUpPart      ); }
	inline ControlPartCode ControlPageDownPart   ()  { return ControlPartCode::Make( kControlPageDownPart    ); }
	
   template<> struct ControlData_Traits< kControlBevelButtonContentTag                  >: POD_DescType_Traits           < ControlButtonContentInfo      > {};
   template<> struct ControlData_Traits< kControlBevelButtonTransformTag                >: POD_DescType_Traits           < IconTransformType             > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextAlignTag                >: POD_DescType_Traits           < ControlButtonTextAlignment    > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextOffsetTag               >: POD_DescType_Traits           < ::SInt16                        > {};
   template<> struct ControlData_Traits< kControlBevelButtonGraphicAlignTag             >: POD_DescType_Traits           < ControlButtonGraphicAlignment > {};
   template<> struct ControlData_Traits< kControlBevelButtonGraphicOffsetTag            >: POD_DescType_Traits           < Point                         > {};
   template<> struct ControlData_Traits< kControlBevelButtonTextPlaceTag                >: POD_DescType_Traits           < ControlButtonTextPlacement    > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuValueTag                >: POD_DescType_Traits           < ::SInt16                        > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuRefTag                  >: POD_DescType_Traits           < MenuRef                       > {};
   template<> struct ControlData_Traits< kControlBevelButtonCenterPopupGlyphTag         >: Converting_DescType_Traits    < bool, Boolean                 > {};
   template<> struct ControlData_Traits< kControlBevelButtonLastMenuTag                 >: POD_DescType_Traits           < ::SInt16                        > {};
   template<> struct ControlData_Traits< kControlBevelButtonMenuDelayTag                >: POD_DescType_Traits           < ::SInt32                        > {};
   template<> struct ControlData_Traits< kControlBevelButtonScaleIconTag                >: Converting_DescType_Traits    < bool, Boolean                 > {};
//   template<> struct ControlData_Traits< kControlBevelButtonOwnedMenuRefTag             >: POD_DescType_Traits           < Nucleus::Owned<MenuRef>                > {};
   template<> struct ControlData_Traits< kControlBevelButtonKindTag                     >: POD_DescType_Traits           < ThemeButtonKind               > {};

   template<> struct ControlData_Traits< kControlTriangleLastValueTag                   >: POD_DescType_Traits           < ::SInt16                        > {};

   template<> struct ControlData_Traits< kControlProgressBarIndeterminateTag            >: Converting_DescType_Traits    < bool, Boolean                 > {};
   template<> struct ControlData_Traits< kControlProgressBarAnimatingTag                >: Converting_DescType_Traits    < bool, Boolean                 > {};

   template<> struct ControlData_Traits< kControlTabContentRectTag                      >: POD_DescType_Traits           < Rect                          > {};
   template<> struct ControlData_Traits< kControlTabEnabledFlagTag                      >: Converting_DescType_Traits    < bool, Boolean                 > {};
   template<> struct ControlData_Traits< kControlTabInfoTag                             >: POD_DescType_Traits           < ControlTabInfoRec             > {};

   template<> struct ControlData_Traits< kControlGroupBoxMenuHandleTag                  >: POD_DescType_Traits           < MenuRef                       > {};
   template<> struct ControlData_Traits< kControlGroupBoxTitleRectTag                   >: POD_DescType_Traits           < Rect                          > {};

   template<> struct ControlData_Traits< kControlImageWellIsDragDestinationTag          >: Converting_DescType_Traits    < bool, Boolean                 > {};

   template<> struct ControlData_Traits< kControlClockLongDateTag                       >: POD_DescType_Traits           < LongDateRec                   > {};

   template<> struct ControlData_Traits< kControlUserItemDrawProcTag                    >: POD_DescType_Traits           < UserItemUPP                   > {};
   template<> struct ControlData_Traits< kControlUserPaneDrawProcTag                    >: POD_DescType_Traits           < ControlUserPaneDrawUPP        > {};
   template<> struct ControlData_Traits< kControlUserPaneHitTestProcTag                 >: POD_DescType_Traits           < ControlUserPaneHitTestUPP     > {};
   template<> struct ControlData_Traits< kControlUserPaneTrackingProcTag                >: POD_DescType_Traits           < ControlUserPaneTrackingUPP    > {};
   template<> struct ControlData_Traits< kControlUserPaneIdleProcTag                    >: POD_DescType_Traits           < ControlUserPaneIdleUPP        > {};
   template<> struct ControlData_Traits< kControlUserPaneKeyDownProcTag                 >: POD_DescType_Traits           < ControlUserPaneKeyDownUPP     > {};
   template<> struct ControlData_Traits< kControlUserPaneActivateProcTag                >: POD_DescType_Traits           < ControlUserPaneActivateUPP    > {};
   template<> struct ControlData_Traits< kControlUserPaneFocusProcTag                   >: POD_DescType_Traits           < ControlUserPaneFocusUPP       > {};
   template<> struct ControlData_Traits< kControlUserPaneBackgroundProcTag              >: POD_DescType_Traits           < ControlUserPaneBackgroundUPP  > {};

   template<> struct ControlData_Traits< kControlEditTextTextTag                        >: DescType_Traits               < ::typeChar                      > {};
   template<> struct ControlData_Traits< kControlEditTextTEHandleTag                    >: POD_DescType_Traits           < TEHandle                      > {};
   template<> struct ControlData_Traits< kControlEditTextSelectionTag                   >: POD_DescType_Traits           < ControlEditTextSelectionRec   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordTag                    >: DescType_Traits               < ::typeChar                      > {};
   template<> struct ControlData_Traits< kControlEditTextKeyScriptBehaviorTag           >: POD_DescType_Traits           < ControlKeyScriptBehavior      > {};
   template<> struct ControlData_Traits< kControlEditTextLockedTag                      >: Converting_DescType_Traits    < bool, Boolean                 > {};
   template<> struct ControlData_Traits< kControlEditTextFixedTextTag                   >: DescType_Traits               < ::typeChar                      > {};
   template<> struct ControlData_Traits< kControlEditTextValidationProcTag              >: POD_DescType_Traits           < ControlEditTextValidationUPP  > {};
   template<> struct ControlData_Traits< kControlEditTextInlinePreUpdateProcTag         >: POD_DescType_Traits           < TSMTEPreUpdateUPP             > {};
   template<> struct ControlData_Traits< kControlEditTextInlinePostUpdateProcTag        >: POD_DescType_Traits           < TSMTEPostUpdateUPP            > {};
   template<> struct ControlData_Traits< kControlEditTextCFStringTag                    >: Seizing_DescType_Traits       < CFStringRef                   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordCFStringTag            >: Seizing_DescType_Traits       < CFStringRef                   > {};

   template<> struct ControlData_Traits< kControlStaticTextTextHeightTag                >: POD_DescType_Traits           < ::SInt16                        > {};
   template<> struct ControlData_Traits< kControlStaticTextTruncTag                     >: POD_DescType_Traits           < TruncCode                     > {};

   template<> struct ControlData_Traits< kControlPictureHandleTag                       >: POD_DescType_Traits           < PicHandle                     > {};

   template<> struct ControlData_Traits< kControlIconTransformTag                       >: POD_DescType_Traits           < IconTransformType             > {};
   template<> struct ControlData_Traits< kControlIconAlignmentTag                       >: POD_DescType_Traits           < IconAlignmentType             > {};
   template<> struct ControlData_Traits< kControlIconResourceIDTag                      >: POD_DescType_Traits           < ::SInt16                        > {};

   template<> struct ControlData_Traits< kControlListBoxListHandleTag                   >: POD_DescType_Traits           < ListHandle                    > {};
   template<> struct ControlData_Traits< kControlListBoxDoubleClickTag                  >: Converting_DescType_Traits    < bool, Boolean                 > {};
   template<> struct ControlData_Traits< kControlListBoxLDEFTag                         >: POD_DescType_Traits           < ::SInt16                        > {};

   template<> struct ControlData_Traits< kControlPushButtonDefaultTag                   >: Converting_DescType_Traits    < bool, Boolean                 > {};
   template<> struct ControlData_Traits< kControlPushButtonCancelTag                    >: Converting_DescType_Traits    < bool, Boolean                 > {};

   template<> struct ControlData_Traits< kControlPopupButtonMenuIDTag                   >: POD_DescType_Traits           < ::SInt16                        > {};
   template<> struct ControlData_Traits< kControlPopupButtonExtraHeightTag              >: POD_DescType_Traits           < ::SInt16                        > {};
   template<> struct ControlData_Traits< kControlPopupButtonCheckCurrentTag             >: Converting_DescType_Traits    < bool, Boolean                 > {};

   template<> struct ControlData_Traits< kControlScrollTextBoxDelayBeforeAutoScrollTag  >: POD_DescType_Traits           < ::UInt32                        > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxDelayBetweenAutoScrollTag >: POD_DescType_Traits           < ::UInt32                        > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxAutoScrollAmountTag       >: POD_DescType_Traits           < ::UInt16                        > {};
   template<> struct ControlData_Traits< kControlScrollTextBoxContentsTag               >: POD_DescType_Traits           < ::SInt16                        > {};

   template<> struct ControlData_Traits< kControlEditUnicodeTextPostUpdateProcTag       >: POD_DescType_Traits           < EditUnicodePostUpdateUPP      > {};
  }

#endif
