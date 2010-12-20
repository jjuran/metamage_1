/*
	Mac/AppleEvents/Types/DescType_scribe.hh
	----------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH
#define MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH

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
	
}

#endif

