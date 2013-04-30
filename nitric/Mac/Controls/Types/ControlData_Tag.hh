/*
	Mac/Controls/Types/ControlData_Tag.hh
	-------------------------------------
*/

#ifndef MAC_CONTROLS_TYPES_CONTROLDATATAG_HH
#define MAC_CONTROLS_TYPES_CONTROLDATATAG_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __CONTROLS__
#include <Controls.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum ControlData_Tag
	{
		// Controls
		kControlFontStyleTag = ::kControlFontStyleTag,
		kControlKeyFilterTag = ::kControlKeyFilterTag,
		kControlKindTag      = ::kControlKindTag,
		kControlSizeTag      = ::kControlSizeTag,
		
		// ControlDefinitions
		kControlBevelButtonContentTag                  = ::kControlBevelButtonContentTag,
		kControlBevelButtonTransformTag                = ::kControlBevelButtonTransformTag,
		kControlBevelButtonTextAlignTag                = ::kControlBevelButtonTextAlignTag,
		kControlBevelButtonTextOffsetTag               = ::kControlBevelButtonTextOffsetTag,
		kControlBevelButtonGraphicAlignTag             = ::kControlBevelButtonGraphicAlignTag,
		kControlBevelButtonGraphicOffsetTag            = ::kControlBevelButtonGraphicOffsetTag,
		kControlBevelButtonTextPlaceTag                = ::kControlBevelButtonTextPlaceTag,
		kControlBevelButtonMenuValueTag                = ::kControlBevelButtonMenuValueTag,
		kControlBevelButtonMenuRefTag                  = ::kControlBevelButtonMenuRefTag,
		kControlBevelButtonCenterPopupGlyphTag         = ::kControlBevelButtonCenterPopupGlyphTag,
		kControlBevelButtonLastMenuTag                 = ::kControlBevelButtonLastMenuTag,
		kControlBevelButtonMenuDelayTag                = ::kControlBevelButtonMenuDelayTag,
		kControlBevelButtonScaleIconTag                = ::kControlBevelButtonScaleIconTag,
		kControlBevelButtonOwnedMenuRefTag             = ::kControlBevelButtonOwnedMenuRefTag,
		kControlBevelButtonKindTag                     = ::kControlBevelButtonKindTag,
		
		kControlTriangleLastValueTag                   = ::kControlTriangleLastValueTag,
		
		kControlProgressBarIndeterminateTag            = ::kControlProgressBarIndeterminateTag,
		kControlProgressBarAnimatingTag                = ::kControlProgressBarAnimatingTag,
		
		kControlTabContentRectTag                      = ::kControlTabContentRectTag,
		kControlTabEnabledFlagTag                      = ::kControlTabEnabledFlagTag,
		kControlTabInfoTag                             = ::kControlTabInfoTag,
		
		kControlGroupBoxMenuHandleTag                  = ::kControlGroupBoxMenuHandleTag,
		kControlGroupBoxTitleRectTag                   = ::kControlGroupBoxTitleRectTag,
		
		kControlImageWellIsDragDestinationTag          = ::kControlImageWellIsDragDestinationTag,
		
		kControlClockLongDateTag                       = ::kControlClockLongDateTag,
		
		kControlUserItemDrawProcTag                    = ::kControlUserItemDrawProcTag,
		kControlUserPaneDrawProcTag                    = ::kControlUserPaneDrawProcTag,
		kControlUserPaneHitTestProcTag                 = ::kControlUserPaneHitTestProcTag,
		kControlUserPaneTrackingProcTag                = ::kControlUserPaneTrackingProcTag,
		kControlUserPaneIdleProcTag                    = ::kControlUserPaneIdleProcTag,
		kControlUserPaneKeyDownProcTag                 = ::kControlUserPaneKeyDownProcTag,
		kControlUserPaneActivateProcTag                = ::kControlUserPaneActivateProcTag,
		kControlUserPaneFocusProcTag                   = ::kControlUserPaneFocusProcTag,
		kControlUserPaneBackgroundProcTag              = ::kControlUserPaneBackgroundProcTag,
		
		kControlEditTextTextTag                        = ::kControlEditTextTextTag,
		kControlEditTextTEHandleTag                    = ::kControlEditTextTEHandleTag,
		kControlEditTextSelectionTag                   = ::kControlEditTextSelectionTag,
		kControlEditTextPasswordTag                    = ::kControlEditTextPasswordTag,
		kControlEditTextKeyScriptBehaviorTag           = ::kControlEditTextKeyScriptBehaviorTag,
		kControlEditTextLockedTag                      = ::kControlEditTextLockedTag,
		kControlEditTextFixedTextTag                   = ::kControlEditTextFixedTextTag,
		kControlEditTextValidationProcTag              = ::kControlEditTextValidationProcTag,
		kControlEditTextInlinePreUpdateProcTag         = ::kControlEditTextInlinePreUpdateProcTag,
		kControlEditTextInlinePostUpdateProcTag        = ::kControlEditTextInlinePostUpdateProcTag,
		kControlEditTextCFStringTag                    = ::kControlEditTextCFStringTag,
		kControlEditTextPasswordCFStringTag            = ::kControlEditTextPasswordCFStringTag,
		
		kControlStaticTextCFStringTag                  = ::kControlStaticTextCFStringTag,
		
		kControlStaticTextTextHeightTag                = ::kControlStaticTextTextHeightTag,
		kControlStaticTextTruncTag                     = ::kControlStaticTextTruncTag,
		
		kControlPictureHandleTag                       = ::kControlPictureHandleTag,
		
		kControlIconTransformTag                       = ::kControlIconTransformTag,
		kControlIconAlignmentTag                       = ::kControlIconAlignmentTag,
		kControlIconResourceIDTag                      = ::kControlIconResourceIDTag,
		
		kControlListBoxListHandleTag                   = ::kControlListBoxListHandleTag,
		kControlListBoxDoubleClickTag                  = ::kControlListBoxDoubleClickTag,
		kControlListBoxLDEFTag                         = ::kControlListBoxLDEFTag,
		
		kControlPushButtonDefaultTag                   = ::kControlPushButtonDefaultTag,
		kControlPushButtonCancelTag                    = ::kControlPushButtonCancelTag,
		
		kControlPopupButtonMenuIDTag                   = ::kControlPopupButtonMenuIDTag,
		kControlPopupButtonExtraHeightTag              = ::kControlPopupButtonExtraHeightTag,
		kControlPopupButtonCheckCurrentTag             = ::kControlPopupButtonCheckCurrentTag,
		
		kControlScrollTextBoxDelayBeforeAutoScrollTag  = ::kControlScrollTextBoxDelayBeforeAutoScrollTag,
		kControlScrollTextBoxDelayBetweenAutoScrollTag = ::kControlScrollTextBoxDelayBetweenAutoScrollTag,
		kControlScrollTextBoxAutoScrollAmountTag       = ::kControlScrollTextBoxAutoScrollAmountTag,
		kControlScrollTextBoxContentsTag               = ::kControlScrollTextBoxContentsTag,
			
		kControlEditUnicodeTextPostUpdateProcTag       = ::kControlEditUnicodeTextPostUpdateProcTag,
			
		kControlData_Tag_Max = nucleus::enumeration_traits< ::FourCharCode >::max
	};
	
}

#endif

