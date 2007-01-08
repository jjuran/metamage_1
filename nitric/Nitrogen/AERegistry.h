// AERegistry.h

#ifndef NITROGEN_AEREGISTRY_H
#define NITROGEN_AEREGISTRY_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEREGISTRY__
#include FRAMEWORK_HEADER(AE,AERegistry.h)
#endif
#ifndef __CARBONEVENTS__
#include FRAMEWORK_HEADER(HIToolbox,CarbonEvents.h)
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_APPLEEVENTS_H
#include "Nitrogen/AppleEvents.h"
#endif

namespace Nitrogen
  {
	
	static const AEKeyword keyAEAngle           = AEKeyword( ::keyAEAngle           );
	static const AEKeyword keyAEArcAngle        = AEKeyword( ::keyAEArcAngle        );
	
	static const AEKeyword keyAEBaseAddr        = AEKeyword( ::keyAEBaseAddr        );
	static const AEKeyword keyAEBestType        = AEKeyword( ::keyAEBestType        );
	static const AEKeyword keyAEBgndColor       = AEKeyword( ::keyAEBgndColor       );
	static const AEKeyword keyAEBgndPattern     = AEKeyword( ::keyAEBgndPattern     );
	static const AEKeyword keyAEBounds          = AEKeyword( ::keyAEBounds          );
	static const AEKeyword keyAECellList        = AEKeyword( ::keyAECellList        );
	static const AEKeyword keyAEClassID         = AEKeyword( ::keyAEClassID         );
	static const AEKeyword keyAEColor           = AEKeyword( ::keyAEColor           );
	static const AEKeyword keyAEColorTable      = AEKeyword( ::keyAEColorTable      );
	static const AEKeyword keyAECurveHeight     = AEKeyword( ::keyAECurveHeight     );
	static const AEKeyword keyAECurveWidth      = AEKeyword( ::keyAECurveWidth      );
	static const AEKeyword keyAEDashStyle       = AEKeyword( ::keyAEDashStyle       );
	static const AEKeyword keyAEData            = AEKeyword( ::keyAEData            );
	static const AEKeyword keyAEDefaultType     = AEKeyword( ::keyAEDefaultType     );
	static const AEKeyword keyAEDefinitionRect  = AEKeyword( ::keyAEDefinitionRect  );
	static const AEKeyword keyAEDescType        = AEKeyword( ::keyAEDescType        );
	static const AEKeyword keyAEDestination     = AEKeyword( ::keyAEDestination     );
	static const AEKeyword keyAEDoAntiAlias     = AEKeyword( ::keyAEDoAntiAlias     );
	static const AEKeyword keyAEDoDithered      = AEKeyword( ::keyAEDoDithered      );
	static const AEKeyword keyAEDoRotate        = AEKeyword( ::keyAEDoRotate        );
	
	static const AEKeyword keyAEDoScale         = AEKeyword( ::keyAEDoScale         );
	static const AEKeyword keyAEDoTranslate     = AEKeyword( ::keyAEDoTranslate     );
	static const AEKeyword keyAEEditionFileLoc  = AEKeyword( ::keyAEEditionFileLoc  );
	static const AEKeyword keyAEElements        = AEKeyword( ::keyAEElements        );
	static const AEKeyword keyAEEndPoint        = AEKeyword( ::keyAEEndPoint        );
	static const AEKeyword keyAEEventClass      = AEKeyword( ::keyAEEventClass      );
	static const AEKeyword keyAEEventID         = AEKeyword( ::keyAEEventID         );
	static const AEKeyword keyAEFile            = AEKeyword( ::keyAEFile            );
	static const AEKeyword keyAEFileType        = AEKeyword( ::keyAEFileType        );
	static const AEKeyword keyAEFillColor       = AEKeyword( ::keyAEFillColor       );
	static const AEKeyword keyAEFillPattern     = AEKeyword( ::keyAEFillPattern     );
	static const AEKeyword keyAEFlipHorizontal  = AEKeyword( ::keyAEFlipHorizontal  );
	static const AEKeyword keyAEFlipVertical    = AEKeyword( ::keyAEFlipVertical    );
	static const AEKeyword keyAEFont            = AEKeyword( ::keyAEFont            );
	static const AEKeyword keyAEFormula         = AEKeyword( ::keyAEFormula         );
	static const AEKeyword keyAEGraphicObjects  = AEKeyword( ::keyAEGraphicObjects  );
	static const AEKeyword keyAEID              = AEKeyword( ::keyAEID              );
	static const AEKeyword keyAEImageQuality    = AEKeyword( ::keyAEImageQuality    );
	static const AEKeyword keyAEInsertHere      = AEKeyword( ::keyAEInsertHere      );
	static const AEKeyword keyAEKeyForms        = AEKeyword( ::keyAEKeyForms        );
	
	static const AEKeyword keyAEKeyword         = AEKeyword( ::keyAEKeyword         );
	static const AEKeyword keyAELevel           = AEKeyword( ::keyAELevel           );
	static const AEKeyword keyAELineArrow       = AEKeyword( ::keyAELineArrow       );
	static const AEKeyword keyAEName            = AEKeyword( ::keyAEName            );
	static const AEKeyword keyAENewElementLoc   = AEKeyword( ::keyAENewElementLoc   );
	static const AEKeyword keyAEObject          = AEKeyword( ::keyAEObject          );
	static const AEKeyword keyAEObjectClass     = AEKeyword( ::keyAEObjectClass     );
	static const AEKeyword keyAEOffStyles       = AEKeyword( ::keyAEOffStyles       );
	static const AEKeyword keyAEOnStyles        = AEKeyword( ::keyAEOnStyles        );
	static const AEKeyword keyAEParameters      = AEKeyword( ::keyAEParameters      );
	static const AEKeyword keyAEParamFlags      = AEKeyword( ::keyAEParamFlags      );
	static const AEKeyword keyAEPenColor        = AEKeyword( ::keyAEPenColor        );
	static const AEKeyword keyAEPenPattern      = AEKeyword( ::keyAEPenPattern      );
	static const AEKeyword keyAEPenWidth        = AEKeyword( ::keyAEPenWidth        );
	static const AEKeyword keyAEPixelDepth      = AEKeyword( ::keyAEPixelDepth      );
	static const AEKeyword keyAEPixMapMinus     = AEKeyword( ::keyAEPixMapMinus     );
	static const AEKeyword keyAEPMTable         = AEKeyword( ::keyAEPMTable         );
	static const AEKeyword keyAEPointList       = AEKeyword( ::keyAEPointList       );
	static const AEKeyword keyAEPointSize       = AEKeyword( ::keyAEPointSize       );
	static const AEKeyword keyAEPosition        = AEKeyword( ::keyAEPosition        );
	
	static const AEKeyword keyAEPropData        = AEKeyword( ::keyAEPropData        );
	static const AEKeyword keyAEProperties      = AEKeyword( ::keyAEProperties      );
	static const AEKeyword keyAEProperty        = AEKeyword( ::keyAEProperty        );
	static const AEKeyword keyAEPropFlags       = AEKeyword( ::keyAEPropFlags       );
	static const AEKeyword keyAEPropID          = AEKeyword( ::keyAEPropID          );
	static const AEKeyword keyAEProtection      = AEKeyword( ::keyAEProtection      );
	static const AEKeyword keyAERenderAs        = AEKeyword( ::keyAERenderAs        );
	static const AEKeyword keyAERequestedType   = AEKeyword( ::keyAERequestedType   );
	static const AEKeyword keyAEResult          = AEKeyword( ::keyAEResult          );
	static const AEKeyword keyAEResultInfo      = AEKeyword( ::keyAEResultInfo      );
	static const AEKeyword keyAERotation        = AEKeyword( ::keyAERotation        );
	static const AEKeyword keyAERotPoint        = AEKeyword( ::keyAERotPoint        );
	static const AEKeyword keyAERowList         = AEKeyword( ::keyAERowList         );
	static const AEKeyword keyAESaveOptions     = AEKeyword( ::keyAESaveOptions     );
	static const AEKeyword keyAEScale           = AEKeyword( ::keyAEScale           );
	static const AEKeyword keyAEScriptTag       = AEKeyword( ::keyAEScriptTag       );
	static const AEKeyword keyAEShowWhere       = AEKeyword( ::keyAEShowWhere       );
	static const AEKeyword keyAEStartAngle      = AEKeyword( ::keyAEStartAngle      );
	static const AEKeyword keyAEStartPoint      = AEKeyword( ::keyAEStartPoint      );
	static const AEKeyword keyAEStyles          = AEKeyword( ::keyAEStyles          );
	
	static const AEKeyword keyAESuiteID         = AEKeyword( ::keyAESuiteID         );
	static const AEKeyword keyAEText            = AEKeyword( ::keyAEText            );
	static const AEKeyword keyAETextColor       = AEKeyword( ::keyAETextColor       );
	static const AEKeyword keyAETextFont        = AEKeyword( ::keyAETextFont        );
	static const AEKeyword keyAETextPointSize   = AEKeyword( ::keyAETextPointSize   );
	static const AEKeyword keyAETextStyles      = AEKeyword( ::keyAETextStyles      );
	static const AEKeyword keyAETextLineHeight  = AEKeyword( ::keyAETextLineHeight  );
	static const AEKeyword keyAETextLineAscent  = AEKeyword( ::keyAETextLineAscent  );
	static const AEKeyword keyAETheText         = AEKeyword( ::keyAETheText         );
	static const AEKeyword keyAETransferMode    = AEKeyword( ::keyAETransferMode    );
	static const AEKeyword keyAETranslation     = AEKeyword( ::keyAETranslation     );
	static const AEKeyword keyAETryAsStructGraf = AEKeyword( ::keyAETryAsStructGraf );
	static const AEKeyword keyAEUniformStyles   = AEKeyword( ::keyAEUniformStyles   );
	static const AEKeyword keyAEUpdateOn        = AEKeyword( ::keyAEUpdateOn        );
	static const AEKeyword keyAEUserTerm        = AEKeyword( ::keyAEUserTerm        );
	static const AEKeyword keyAEWindow          = AEKeyword( ::keyAEWindow          );
	static const AEKeyword keyAEWritingCode     = AEKeyword( ::keyAEWritingCode     );
	
	static const AEKeyword keyMiscellaneous     = AEKeyword( ::keyMiscellaneous     );
	static const AEKeyword keySelection         = AEKeyword( ::keySelection         );
	static const AEKeyword keyWindow            = AEKeyword( ::keyWindow            );
	
	static const AEKeyword keyWhen              = AEKeyword( ::keyWhen              );
	static const AEKeyword keyWhere             = AEKeyword( ::keyWhere             );
	static const AEKeyword keyModifiers         = AEKeyword( ::keyModifiers         );
	static const AEKeyword keyKey               = AEKeyword( ::keyKey               );
	static const AEKeyword keyKeyCode           = AEKeyword( ::keyKeyCode           );
	static const AEKeyword keyKeyboard          = AEKeyword( ::keyKeyboard          );
	static const AEKeyword keyDriveNumber       = AEKeyword( ::keyDriveNumber       );
	static const AEKeyword keyErrorCode         = AEKeyword( ::keyErrorCode         );
	static const AEKeyword keyHighLevelClass    = AEKeyword( ::keyHighLevelClass    );
	static const AEKeyword keyHighLevelID       = AEKeyword( ::keyHighLevelID       );
	
   template<> struct DescType_Traits< typeFixedPoint                >: Nucleus::PodFlattener< FixedPoint                > {};
   template<> struct DescType_Traits< typeFixedRectangle            >: Nucleus::PodFlattener< FixedRect                 > {};
   template<> struct DescType_Traits< typeIntlWritingCode           >: Nucleus::PodFlattener< ScriptCode                > {};
   template<> struct DescType_Traits< typePtr                       >: Nucleus::PodFlattener< void *                    > {};
   template<> struct DescType_Traits< typeQDPoint                   >: Nucleus::PodFlattener< Point                     > {};
   template<> struct DescType_Traits< typeRectangle                 >: Nucleus::PodFlattener< Rect                      > {};
   template<> struct DescType_Traits< typeRGBColor                  >: Nucleus::PodFlattener< RGBColor                  > {};
   template<> struct DescType_Traits< typeScript                    >: Nucleus::PodFlattener< ScriptCode                > {};
   template<> struct DescType_Traits< typeTextRange                 >: Nucleus::PodFlattener< TextRange                 > {};
   template<> struct DescType_Traits< typeComponentInstance         >: Nucleus::PodFlattener< ComponentInstance         > {};
   template<> struct DescType_Traits< typeEventRef                  >: Nucleus::PodFlattener< EventRef                  > {};

   template<> struct DescType_Traits< typeUnicodeText >: UnicodeFlattener< UniChar > {};
	
	using ::VersRec;
	
	inline std::size_t SizeOf_VersRec( const VersRec& version )
	{
		UInt8 shortVersionLength = version.shortVersion[ 0 ];
		
		// The long version string immediately follows the short version string.
		const UInt8* longVersion = version.shortVersion + 1 + shortVersionLength;
		UInt8 longVersionLength  = longVersion[ 0 ];
		
		return sizeof (::NumVersion)
		     + sizeof (SInt16)
		     + 1 + shortVersionLength
		     + 1 + longVersionLength;
	}
	
   using ::OffsetArray;

   inline std::size_t SizeOf_OffsetArray( const OffsetArray& array )
     {
      return sizeof( array ) - sizeof( array.fOffset ) + array.fNumOfOffsets * sizeof( long );
     }
   
   using ::TextRangeArray;

   inline std::size_t SizeOf_TextRangeArray( const TextRangeArray& array )
     {
      return sizeof( array ) - sizeof( array.fRange ) + array.fNumOfRanges * sizeof( TextRange );
     }
   
   template<> struct DescType_Traits< typeVersion        >: Nucleus::VariableLengthPodFlattener< VersRec,        SizeOf_VersRec        > {};
   template<> struct DescType_Traits< typeOffsetArray    >: Nucleus::VariableLengthPodFlattener< OffsetArray,    SizeOf_OffsetArray    > {};
   template<> struct DescType_Traits< typeTextRangeArray >: Nucleus::VariableLengthPodFlattener< TextRangeArray, SizeOf_TextRangeArray > {};
  }

#endif
