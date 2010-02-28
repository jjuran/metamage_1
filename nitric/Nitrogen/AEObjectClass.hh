// Nitrogen/AEObjectClass.hh
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_AEOBJECTCLASS_HH
#define NITROGEN_AEOBJECTCLASS_HH

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Nitrogen
{
	
	enum AEObjectClass
	{
		
		// AERegistry
		// ----------
		
		cAEList             = ::cAEList,
		cApplication        = ::cApplication,
		cArc                = ::cArc,
		cBoolean            = ::cBoolean,
		cCell               = ::cCell,
		cChar               = ::cChar,
		cColorTable         = ::cColorTable,
		cColumn             = ::cColumn,
		cDocument           = ::cDocument,
		cDrawingArea        = ::cDrawingArea,
		cEnumeration        = ::cEnumeration,
		cFile               = ::cFile,
		cFixed              = ::cFixed,
		cFixedPoint         = ::cFixedPoint,
		cFixedRectangle     = ::cFixedRectangle,
		cGraphicLine        = ::cGraphicLine,
		cGraphicObject      = ::cGraphicObject,
		cGraphicShape       = ::cGraphicShape,
		cGraphicText        = ::cGraphicText,
		cGroupedGraphic     = ::cGroupedGraphic,
	
	
		cInsertionLoc       = ::cInsertionLoc,
		cInsertionPoint     = ::cInsertionPoint,
		cIntlText           = ::cIntlText,
		cIntlWritingCode    = ::cIntlWritingCode,
		cItem               = ::cItem,
		cLine               = ::cLine,
		cLongDateTime       = ::cLongDateTime,
		cLongFixed          = ::cLongFixed,
		cLongFixedPoint     = ::cLongFixedPoint,
		cLongFixedRectangle = ::cLongFixedRectangle,
		cLongInteger        = ::cLongInteger,
		cLongPoint          = ::cLongPoint,
		cLongRectangle      = ::cLongRectangle,
		cMachineLoc         = ::cMachineLoc,
		cMenu               = ::cMenu,
		cMenuItem           = ::cMenuItem,
		cObject             = ::cObject,
		cObjectSpecifier    = ::cObjectSpecifier,
		cOpenableObject     = ::cOpenableObject,
		cOval               = ::cOval,
	
	
		cParagraph          = ::cParagraph,
		cPICT               = ::cPICT,
		cPixel              = ::cPixel,
		cPixelMap           = ::cPixelMap,
		cPolygon            = ::cPolygon,
		cProperty           = ::cProperty,
		cQDPoint            = ::cQDPoint,
		cQDRectangle        = ::cQDRectangle,
		cRectangle          = ::cRectangle,
		cRGBColor           = ::cRGBColor,
		cRotation           = ::cRotation,
		cRoundedRectangle   = ::cRoundedRectangle,
		cRow                = ::cRow,
		cSelection          = ::cSelection,
		cShortInteger       = ::cShortInteger,
		cTable              = ::cTable,
		cText               = ::cText,
		cTextFlow           = ::cTextFlow,
		cTextStyles         = ::cTextStyles,
		cType               = ::cType,
		
		cVersion            = ::cVersion,
		cWindow             = ::cWindow,
		cWord               = ::cWord,
		
		// ASRegistry
		// ----------
		
		cConstant               = ::cConstant,
		cClassIdentifier        = ::cClassIdentifier,
		cObjectBeingExamined    = ::cObjectBeingExamined,
		cList                   = ::cList,
		cSmallReal              = ::cSmallReal,
		cReal                   = ::cReal,
		cRecord                 = ::cRecord,
		cReference              = ::cReference,
		cUndefined              = ::cUndefined,
		cMissingValue           = ::cMissingValue,
		cSymbol                 = ::cSymbol,
		cLinkedList             = ::cLinkedList,
		cVector                 = ::cVector,
		cEventIdentifier        = ::cEventIdentifier,
		cKeyIdentifier          = ::cKeyIdentifier,
		cUserIdentifier         = ::cUserIdentifier,
		cPreposition            = ::cPreposition,
		cKeyForm                = ::cKeyForm,
		cScript                 = ::cScript,
		cHandler                = ::cHandler,
		cProcedure              = ::cProcedure,
		cHandleBreakpoint       = ::cHandleBreakpoint,
		cClosure                = ::cClosure,
		cRawData                = ::cRawData,
		cStringClass            = ::cStringClass,
		cNumber                 = ::cNumber,
		cListElement            = ::cListElement,
		cListOrRecord           = ::cListOrRecord,
		cListOrString           = ::cListOrString,
		cListRecordOrString     = ::cListRecordOrString,
		cNumberOrString         = ::cNumberOrString,
		cNumberOrDateTime       = ::cNumberOrDateTime,
		cNumberDateTimeOrString = ::cNumberDateTimeOrString,
		cAliasOrString          = ::cAliasOrString,
		cSeconds                = ::cSeconds,
		
		kAEObjectClass_Max = nucleus::enumeration_traits< ::DescType >::max
	};
	
}

#endif

