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

   template<> struct ControlData_Traits< kControlEditTextTextTag                        >: DescType_Traits        < ::typeChar                    > {};
   template<> struct ControlData_Traits< kControlEditTextTEHandleTag                    >: Nucleus::PodFlattener    < TEHandle                      > {};
   template<> struct ControlData_Traits< kControlEditTextSelectionTag                   >: Nucleus::PodFlattener    < ControlEditTextSelectionRec   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordTag                    >: DescType_Traits        < ::typeChar                    > {};
   template<> struct ControlData_Traits< kControlEditTextKeyScriptBehaviorTag           >: Nucleus::PodFlattener    < ControlKeyScriptBehavior      > {};
   template<> struct ControlData_Traits< kControlEditTextLockedTag                      >: BooleanFlattener                                           {};
   template<> struct ControlData_Traits< kControlEditTextFixedTextTag                   >: DescType_Traits        < ::typeChar                    > {};
   template<> struct ControlData_Traits< kControlEditTextValidationProcTag              >: Nucleus::PodFlattener    < ControlEditTextValidationUPP  > {};
   template<> struct ControlData_Traits< kControlEditTextInlinePreUpdateProcTag         >: Nucleus::PodFlattener    < TSMTEPreUpdateUPP             > {};
   template<> struct ControlData_Traits< kControlEditTextInlinePostUpdateProcTag        >: Nucleus::PodFlattener    < TSMTEPostUpdateUPP            > {};
   template<> struct ControlData_Traits< kControlEditTextCFStringTag                    >: Nucleus::SeizingPODFlattener< CFStringRef                   > {};
   template<> struct ControlData_Traits< kControlEditTextPasswordCFStringTag            >: Nucleus::SeizingPODFlattener< CFStringRef                   > {};

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
  }

#endif
