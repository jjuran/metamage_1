/*
	Mac/AppleEvents/Types/DescType_scribe.hh
	----------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH
#define MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH

// Mac OS
#ifndef __APPEARANCE__
#include <Appearance.h>
#endif
#ifndef __SCRAP__
#include <Scrap.h>
#endif

#if !TARGET_API_MAC_CARBON
#ifndef __EPPC__
#include <EPPC.h>
#endif
#endif

// nucleus
#ifndef NUCLEUS_SCRIBE_HH
#include "nucleus/scribe.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEABSOLUTEORDINAL_HH
#include "Mac/AppleEvents/Types/AEAbsoluteOrdinal.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEENUMERATION_HH
#include "Mac/AppleEvents/Types/AEEnumeration.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYWORD_HH
#include "Mac/AppleEvents/Types/AEKeyword.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif
#ifndef MAC_FILES_TYPES_FSCREATOR_HH
#include "Mac/Files/Types/FSCreator.hh"
#endif


namespace Mac
{
	
	template < DescType > struct DescType_scribe;
	
	typedef nucleus::converting_POD_scribe< bool, ::Boolean > Boolean_scribe;
	
	// AEDataModel
	// -----------
	
	template <> struct DescType_scribe< typeBoolean > : Boolean_scribe {};
	
	template <> struct DescType_scribe< typeSInt16 >                 : nucleus::POD_scribe< SInt16 >      {};
	template <> struct DescType_scribe< typeSInt32 >                 : nucleus::POD_scribe< SInt32 >      {};
	template <> struct DescType_scribe< typeUInt32 >                 : nucleus::POD_scribe< UInt32 >      {};
	template <> struct DescType_scribe< typeSInt64 >                 : nucleus::POD_scribe< SInt64 >      {};
	template <> struct DescType_scribe< typeIEEE32BitFloatingPoint > : nucleus::POD_scribe< float >       {};
	template <> struct DescType_scribe< typeIEEE64BitFloatingPoint > : nucleus::POD_scribe< double >      {};
	template <> struct DescType_scribe< type128BitFloatingPoint >    : nucleus::POD_scribe< long double > {};
	
	template <> struct DescType_scribe< typeEventRecord         > : nucleus::POD_scribe< EventRecord         > {};
	template <> struct DescType_scribe< typeFSS                 > : nucleus::POD_scribe< FSSpec              > {};
	template <> struct DescType_scribe< typeFSRef               > : nucleus::POD_scribe< FSRef               > {};
	template <> struct DescType_scribe< typeQDRectangle         > : nucleus::POD_scribe< Rect                > {};
	template <> struct DescType_scribe< typeProcessSerialNumber > : nucleus::POD_scribe< ProcessSerialNumber > {};
	
#if !TARGET_API_MAC_CARBON
	
	// TargetID is defined for Carbon, but typeTargetID is not.
	template <> struct DescType_scribe< typeTargetID > : nucleus::POD_scribe< TargetID > {};
	
#endif
	
	template <> struct DescType_scribe< typeEnumerated    > : nucleus::converting_POD_scribe< AEEnumeration, ::DescType  > {};
	template <> struct DescType_scribe< typeType          > : nucleus::converting_POD_scribe< DescType,      ::DescType  > {};
	template <> struct DescType_scribe< typeKeyword       > : nucleus::converting_POD_scribe< AEKeyword,     ::AEKeyword > {};
	template <> struct DescType_scribe< typeApplSignature > : nucleus::converting_POD_scribe< FSCreator,     ::OSType    > {};
	
	// AEObjects
	// ---------
	
	template <> struct DescType_scribe< typeAbsoluteOrdinal > : nucleus::converting_POD_scribe< AEAbsoluteOrdinal, ::DescType > {};
	
	// AERegistry
	// ----------
	
	template <> struct DescType_scribe< typeFixedPoint        > : nucleus::POD_scribe< FixedPoint        > {};
	template <> struct DescType_scribe< typeFixedRectangle    > : nucleus::POD_scribe< FixedRect         > {};
	template <> struct DescType_scribe< typeIntlWritingCode   > : nucleus::POD_scribe< ScriptCode        > {};
	template <> struct DescType_scribe< typePtr               > : nucleus::POD_scribe< void *            > {};
	template <> struct DescType_scribe< typeQDPoint           > : nucleus::POD_scribe< Point             > {};
	template <> struct DescType_scribe< typeRectangle         > : nucleus::POD_scribe< Rect              > {};
	template <> struct DescType_scribe< typeRGBColor          > : nucleus::POD_scribe< RGBColor          > {};
	template <> struct DescType_scribe< typeScript            > : nucleus::POD_scribe< ScriptCode        > {};
	template <> struct DescType_scribe< typeTextRange         > : nucleus::POD_scribe< TextRange         > {};
	template <> struct DescType_scribe< typeComponentInstance > : nucleus::POD_scribe< ComponentInstance > {};
	template <> struct DescType_scribe< typeEventRef          > : nucleus::POD_scribe< EventRef          > {};
	
	// CarbonEvents
	// ------------
	
	template <> struct DescType_scribe< typeWindowRef               > : nucleus::POD_scribe< WindowRef               > {};
	template <> struct DescType_scribe< typeGrafPtr                 > : nucleus::POD_scribe< CGrafPtr                > {};
	template <> struct DescType_scribe< typeGWorldPtr               > : nucleus::POD_scribe< GWorldPtr               > {};
	template <> struct DescType_scribe< typeDragRef                 > : nucleus::POD_scribe< DragRef                 > {};
	template <> struct DescType_scribe< typeMenuRef                 > : nucleus::POD_scribe< MenuRef                 > {};
	template <> struct DescType_scribe< typeControlRef              > : nucleus::POD_scribe< ControlRef              > {};
	template <> struct DescType_scribe< typeCollection              > : nucleus::POD_scribe< Collection              > {};
	template <> struct DescType_scribe< typeQDRgnHandle             > : nucleus::POD_scribe< RgnHandle               > {};
	template <> struct DescType_scribe< typeOSStatus                > : nucleus::POD_scribe< OSStatus                > {};
	template <> struct DescType_scribe< typeCFStringRef             > : nucleus::POD_scribe< CFStringRef             > {};
	template <> struct DescType_scribe< typeCGContextRef            > : nucleus::POD_scribe< CGContextRef            > {};
	template <> struct DescType_scribe< typeHIPoint                 > : nucleus::POD_scribe< HIPoint                 > {};
	
	template <> struct DescType_scribe< typeMouseButton             > : nucleus::POD_scribe< EventMouseButton        > {};
	template <> struct DescType_scribe< typeMouseWheelAxis          > : nucleus::POD_scribe< EventMouseWheelAxis     > {};
	
	template <> struct DescType_scribe< typeEventHotKeyID           > : nucleus::POD_scribe< EventHotKeyID           > {};
	
	template <> struct DescType_scribe< typeHICommand               > : nucleus::POD_scribe< HICommand               > {};
	
	template <> struct DescType_scribe< typeWindowRegionCode        > : nucleus::POD_scribe< WindowRegionCode        > {};
	template <> struct DescType_scribe< typeWindowDefPartCode       > : nucleus::POD_scribe< WindowDefPartCode       > {};
	template <> struct DescType_scribe< typeClickActivationResult   > : nucleus::POD_scribe< ClickActivationResult   > {};
	
	template <> struct DescType_scribe< typeControlActionUPP        > : nucleus::POD_scribe< ControlActionUPP        > {};
	template <> struct DescType_scribe< typeIndicatorDragConstraint > : nucleus::POD_scribe< IndicatorDragConstraint > {};
	template <> struct DescType_scribe< typeControlPartCode         > : nucleus::POD_scribe< ControlPartCode         > {};
	
	template <> struct DescType_scribe< typeMenuItemIndex           > : nucleus::POD_scribe< MenuItemIndex           > {};
	template <> struct DescType_scribe< typeMenuCommand             > : nucleus::POD_scribe< MenuCommand             > {};
	template <> struct DescType_scribe< typeMenuTrackingMode        > : nucleus::POD_scribe< MenuTrackingMode        > {};
	template <> struct DescType_scribe< typeMenuEventOptions        > : nucleus::POD_scribe< MenuEventOptions        > {};
	template <> struct DescType_scribe< typeThemeMenuState          > : nucleus::POD_scribe< ThemeMenuState          > {};
	template <> struct DescType_scribe< typeThemeMenuItemType       > : nucleus::POD_scribe< ThemeMenuItemType       > {};
	
	template <> struct DescType_scribe< typeTabletPointRec          > : nucleus::POD_scribe< TabletPointRec          > {};
	template <> struct DescType_scribe< typeTabletProximityRec      > : nucleus::POD_scribe< TabletProximityRec      > {};
	
	template <> struct DescType_scribe< typeScrapRef                > : nucleus::POD_scribe< ScrapRef                > {};
	template <> struct DescType_scribe< typeCFMutableArrayRef       > : nucleus::POD_scribe< CFMutableArrayRef       > {};
	
	template <> struct DescType_scribe< typeFSVolumeRefNum          > : nucleus::POD_scribe< FSVolumeRefNum          > {};
	
	// This one is different; N::CFTypeRef is a type defined in Nitrogen; so we use converting_POD_scribe
	template <> struct DescType_scribe< typeCFTypeRef               > : nucleus::converting_POD_scribe< CFTypeRef, ::CFTypeRef > {};
	
#ifdef __MACH__
	
	// HIObject
	// --------
	
	template <> struct DescType_scribe< typeHIObjectRef > : nucleus::POD_scribe< HIObjectRef > {};
	
	// HIToolbar
	// --------
	
	template <> struct DescType_scribe< typeHIToolbarRef     > : nucleus::POD_scribe< HIToolbarRef     > {};
	template <> struct DescType_scribe< typeHIToolbarItemRef > : nucleus::POD_scribe< HIToolbarItemRef > {};
	
#endif
	
}

#endif

