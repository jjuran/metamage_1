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

// iota
#ifndef IOTA_TYPE_HH
#include "iota/type.hh"
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
#ifndef MAC_TOOLBOX_TYPES_BOOLEAN_HH
#include "Mac/Toolbox/Types/Boolean.hh"
#endif


namespace Mac
{
	
	using iota::type_;
	
	
	template < DescType descType >
	struct DescType_
	{
		static const DescType value = descType;
	};
	
	
	template < class Integer > struct Integer_DescType;
	
	template <> struct Integer_DescType< SInt16 > : DescType_< typeSInt16 > {};
	template <> struct Integer_DescType< SInt32 > : DescType_< typeSInt32 > {};
	template <> struct Integer_DescType< UInt32 > : DescType_< typeUInt32 > {};
	template <> struct Integer_DescType< SInt64 > : DescType_< typeSInt64 > {};
	
	
	template < DescType > struct DescType_integer;
	
	template <> struct DescType_integer< typeSInt16 > : type_< SInt16 > {};
	template <> struct DescType_integer< typeSInt32 > : type_< SInt32 > {};
	template <> struct DescType_integer< typeUInt32 > : type_< UInt32 > {};
	
	template <> struct DescType_integer< typeType       > : type_< ::DescType > {};
	template <> struct DescType_integer< typeEnumerated > : type_< ::DescType > {};
	
	
	template < DescType > struct DescType_scribe;
	
	// AEDataModel
	// -----------
	
	// This is not a valid scribe, but the minimum needed by CarbonEvents
	template <> struct DescType_scribe< typeNull > { struct type { typedef void Result; }; };
	
	template <> struct DescType_scribe< typeBoolean > : type_< Boolean_scribe > {};
	
	template <> struct DescType_scribe< typeSInt16 >                 : type_< nucleus::POD_scribe< SInt16 >      > {};
	template <> struct DescType_scribe< typeSInt32 >                 : type_< nucleus::POD_scribe< SInt32 >      > {};
	template <> struct DescType_scribe< typeUInt32 >                 : type_< nucleus::POD_scribe< UInt32 >      > {};
	template <> struct DescType_scribe< typeSInt64 >                 : type_< nucleus::POD_scribe< SInt64 >      > {};
	template <> struct DescType_scribe< typeIEEE32BitFloatingPoint > : type_< nucleus::POD_scribe< float >       > {};
	template <> struct DescType_scribe< typeIEEE64BitFloatingPoint > : type_< nucleus::POD_scribe< double >      > {};
	template <> struct DescType_scribe< type128BitFloatingPoint >    : type_< nucleus::POD_scribe< long double > > {};
	
	template <> struct DescType_scribe< typeEventRecord         > : type_< nucleus::POD_scribe< EventRecord         > > {};
	template <> struct DescType_scribe< typeFSS                 > : type_< nucleus::POD_scribe< FSSpec              > > {};
	template <> struct DescType_scribe< typeFSRef               > : type_< nucleus::POD_scribe< FSRef               > > {};
	template <> struct DescType_scribe< typeQDRectangle         > : type_< nucleus::POD_scribe< Rect                > > {};
	template <> struct DescType_scribe< typeProcessSerialNumber > : type_< nucleus::POD_scribe< ProcessSerialNumber > > {};
	
#if !TARGET_API_MAC_CARBON
	
	// TargetID is defined for Carbon, but typeTargetID is not.
	template <> struct DescType_scribe< typeTargetID > : type_< nucleus::POD_scribe< TargetID > > {};
	
#endif
	
	template <> struct DescType_scribe< typeEnumerated    > : type_< nucleus::converting_POD_scribe< AEEnumeration, ::DescType  > > {};
	template <> struct DescType_scribe< typeType          > : type_< nucleus::converting_POD_scribe< DescType,      ::DescType  > > {};
	template <> struct DescType_scribe< typeKeyword       > : type_< nucleus::converting_POD_scribe< AEKeyword,     ::AEKeyword > > {};
	template <> struct DescType_scribe< typeApplSignature > : type_< nucleus::converting_POD_scribe< FSCreator,     ::OSType    > > {};
	
	// AEObjects
	// ---------
	
	template <> struct DescType_scribe< typeAbsoluteOrdinal > : type_< nucleus::converting_POD_scribe< AEAbsoluteOrdinal, ::DescType > > {};
	
	// AERegistry
	// ----------
	
	template <> struct DescType_scribe< typeFixedPoint        > : type_< nucleus::POD_scribe< FixedPoint        > > {};
	template <> struct DescType_scribe< typeFixedRectangle    > : type_< nucleus::POD_scribe< FixedRect         > > {};
	template <> struct DescType_scribe< typeIntlWritingCode   > : type_< nucleus::POD_scribe< ScriptCode        > > {};
	template <> struct DescType_scribe< typePtr               > : type_< nucleus::POD_scribe< void *            > > {};
	template <> struct DescType_scribe< typeQDPoint           > : type_< nucleus::POD_scribe< Point             > > {};
	template <> struct DescType_scribe< typeRectangle         > : type_< nucleus::POD_scribe< Rect              > > {};
	template <> struct DescType_scribe< typeRGBColor          > : type_< nucleus::POD_scribe< RGBColor          > > {};
	template <> struct DescType_scribe< typeScript            > : type_< nucleus::POD_scribe< ScriptCode        > > {};
	template <> struct DescType_scribe< typeTextRange         > : type_< nucleus::POD_scribe< TextRange         > > {};
	template <> struct DescType_scribe< typeComponentInstance > : type_< nucleus::POD_scribe< ComponentInstance > > {};
	template <> struct DescType_scribe< typeEventRef          > : type_< nucleus::POD_scribe< EventRef          > > {};
	
	// CarbonEvents
	// ------------
	
	template <> struct DescType_scribe< typeWindowRef               > : type_< nucleus::POD_scribe< WindowRef               > > {};
	template <> struct DescType_scribe< typeGrafPtr                 > : type_< nucleus::POD_scribe< CGrafPtr                > > {};
	template <> struct DescType_scribe< typeGWorldPtr               > : type_< nucleus::POD_scribe< GWorldPtr               > > {};
	template <> struct DescType_scribe< typeDragRef                 > : type_< nucleus::POD_scribe< DragRef                 > > {};
	template <> struct DescType_scribe< typeMenuRef                 > : type_< nucleus::POD_scribe< MenuRef                 > > {};
	template <> struct DescType_scribe< typeControlRef              > : type_< nucleus::POD_scribe< ControlRef              > > {};
	template <> struct DescType_scribe< typeCollection              > : type_< nucleus::POD_scribe< Collection              > > {};
	template <> struct DescType_scribe< typeQDRgnHandle             > : type_< nucleus::POD_scribe< RgnHandle               > > {};
	template <> struct DescType_scribe< typeOSStatus                > : type_< nucleus::POD_scribe< OSStatus                > > {};
	template <> struct DescType_scribe< typeCFStringRef             > : type_< nucleus::POD_scribe< CFStringRef             > > {};
	template <> struct DescType_scribe< typeCGContextRef            > : type_< nucleus::POD_scribe< CGContextRef            > > {};
	template <> struct DescType_scribe< typeHIPoint                 > : type_< nucleus::POD_scribe< HIPoint                 > > {};
	
	template <> struct DescType_scribe< typeMouseButton             > : type_< nucleus::POD_scribe< EventMouseButton        > > {};
	template <> struct DescType_scribe< typeMouseWheelAxis          > : type_< nucleus::POD_scribe< EventMouseWheelAxis     > > {};
	
	template <> struct DescType_scribe< typeEventHotKeyID           > : type_< nucleus::POD_scribe< EventHotKeyID           > > {};
	
	template <> struct DescType_scribe< typeHICommand               > : type_< nucleus::POD_scribe< HICommand               > > {};
	
	template <> struct DescType_scribe< typeWindowRegionCode        > : type_< nucleus::POD_scribe< WindowRegionCode        > > {};
	template <> struct DescType_scribe< typeWindowDefPartCode       > : type_< nucleus::POD_scribe< WindowDefPartCode       > > {};
	template <> struct DescType_scribe< typeClickActivationResult   > : type_< nucleus::POD_scribe< ClickActivationResult   > > {};
	
	template <> struct DescType_scribe< typeControlActionUPP        > : type_< nucleus::POD_scribe< ControlActionUPP        > > {};
	template <> struct DescType_scribe< typeIndicatorDragConstraint > : type_< nucleus::POD_scribe< IndicatorDragConstraint > > {};
	template <> struct DescType_scribe< typeControlPartCode         > : type_< nucleus::POD_scribe< ControlPartCode         > > {};
	
	template <> struct DescType_scribe< typeMenuItemIndex           > : type_< nucleus::POD_scribe< MenuItemIndex           > > {};
	template <> struct DescType_scribe< typeMenuCommand             > : type_< nucleus::POD_scribe< MenuCommand             > > {};
	template <> struct DescType_scribe< typeMenuTrackingMode        > : type_< nucleus::POD_scribe< MenuTrackingMode        > > {};
	template <> struct DescType_scribe< typeMenuEventOptions        > : type_< nucleus::POD_scribe< MenuEventOptions        > > {};
	template <> struct DescType_scribe< typeThemeMenuState          > : type_< nucleus::POD_scribe< ThemeMenuState          > > {};
	template <> struct DescType_scribe< typeThemeMenuItemType       > : type_< nucleus::POD_scribe< ThemeMenuItemType       > > {};
	
	template <> struct DescType_scribe< typeTabletPointRec          > : type_< nucleus::POD_scribe< TabletPointRec          > > {};
	template <> struct DescType_scribe< typeTabletProximityRec      > : type_< nucleus::POD_scribe< TabletProximityRec      > > {};
	
	template <> struct DescType_scribe< typeScrapRef                > : type_< nucleus::POD_scribe< ScrapRef                > > {};
	template <> struct DescType_scribe< typeCFMutableArrayRef       > : type_< nucleus::POD_scribe< CFMutableArrayRef       > > {};
	
	template <> struct DescType_scribe< typeFSVolumeRefNum          > : type_< nucleus::POD_scribe< FSVolumeRefNum          > > {};
	
	// This one is different; N::CFTypeRef is a type defined in Nitrogen; so we use converting_POD_scribe
	template <> struct DescType_scribe< typeCFTypeRef               > : type_< nucleus::converting_POD_scribe< CFTypeRef, ::CFTypeRef > > {};
	
#ifdef __MACH__
	
	// HIObject
	// --------
	
	template <> struct DescType_scribe< typeHIObjectRef > : type_< nucleus::POD_scribe< HIObjectRef > > {};
	
	// HIToolbar
	// --------
	
	template <> struct DescType_scribe< typeHIToolbarRef     > : type_< nucleus::POD_scribe< HIToolbarRef     > > {};
	template <> struct DescType_scribe< typeHIToolbarItemRef > : type_< nucleus::POD_scribe< HIToolbarItemRef > > {};
	
#endif
	
}

#endif

