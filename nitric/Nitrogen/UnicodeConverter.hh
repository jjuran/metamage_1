// Nitrogen/UnicodeConverter.hh
// ----------------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_UNICODECONVERTER_HH
#define NITROGEN_UNICODECONVERTER_HH


#ifndef __UNICODECONVERTER__
#include <UnicodeConverter.h>
#endif

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif

#ifndef NITROGEN_TEXTCOMMON_HH
#include "Nitrogen/TextCommon.hh"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif


namespace Nitrogen {
	
//	We're skipping the UPP stuff for now.....
#if 0
/*
 *  NewUnicodeToTextFallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern UnicodeToTextFallbackUPP
NewUnicodeToTextFallbackUPP(UnicodeToTextFallbackProcPtr userRoutine) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  DisposeUnicodeToTextFallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
DisposeUnicodeToTextFallbackUPP(UnicodeToTextFallbackUPP userUPP) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  InvokeUnicodeToTextFallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern OSStatus
InvokeUnicodeToTextFallbackUPP(
  UniChar *                 iSrcUniStr,
  ByteCount                 iSrcUniStrLen,
  ByteCount *               oSrcConvLen,
  TextPtr                   oDestStr,
  ByteCount                 iDestStrLen,
  ByteCount *               oDestConvLen,
  LogicalAddress            iInfoPtr,
  ConstUnicodeMappingPtr    iUnicodeMappingPtr,
  UnicodeToTextFallbackUPP  userUPP)                          AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif

  using ::TextToUnicodeInfo;
  using ::UnicodeToTextInfo;
  using ::UnicodeToTextRunInfo;
  }


namespace Nucleus
  {
//	Do these need to take pointers???? [ No - they are already pointers ]
	template <> struct Disposer< Nitrogen::TextToUnicodeInfo >: public std::unary_function< Nitrogen::TextToUnicodeInfo, void >
		{
		void operator()( Nitrogen::TextToUnicodeInfo ttui ) const
			{
				(void) ::DisposeTextToUnicodeInfo ( &ttui );
			}
		};

	template <> struct Disposer< Nitrogen::UnicodeToTextInfo >: public std::unary_function< Nitrogen::UnicodeToTextInfo, void >
		{
		void operator()( Nitrogen::UnicodeToTextInfo utti ) const
			{
				(void) ::DisposeUnicodeToTextInfo ( &utti );
			}
		};

	template <> struct Disposer< Nitrogen::UnicodeToTextRunInfo >: public std::unary_function< Nitrogen::UnicodeToTextRunInfo, void >
		{
		void operator()( Nitrogen::UnicodeToTextRunInfo uttri ) const
			{
				(void) ::DisposeUnicodeToTextRunInfo ( &uttri );
			}
		};
  }

namespace Nitrogen
  {

//	Shouldn't these pointers be const &s ??
	inline Nucleus::Owned<TextToUnicodeInfo> CreateTextToUnicodeInfo ( const UnicodeMapping &iUnicodeMapping ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		TextToUnicodeInfo result;
		ThrowOSStatus ( ::CreateTextToUnicodeInfo ( &iUnicodeMapping, &result ));
		return Nucleus::Owned<TextToUnicodeInfo>::Seize ( result );
		}
	
	inline Nucleus::Owned<TextToUnicodeInfo> CreateTextToUnicodeInfoByEncoding ( TextEncoding iEncoding ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		TextToUnicodeInfo result;
		ThrowOSStatus ( ::CreateTextToUnicodeInfoByEncoding ( iEncoding, &result ));
		return Nucleus::Owned<TextToUnicodeInfo>::Seize ( result );
		}

	inline Nucleus::Owned<UnicodeToTextInfo> CreateUnicodeToTextInfo ( const UnicodeMapping &iUnicodeMapping ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		UnicodeToTextInfo result;
		ThrowOSStatus ( ::CreateUnicodeToTextInfo ( &iUnicodeMapping, &result ));
		return Nucleus::Owned<UnicodeToTextInfo>::Seize ( result );
		}
	
	inline Nucleus::Owned<UnicodeToTextInfo> CreateUnicodeToTextInfoByEncoding ( TextEncoding iEncoding ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		UnicodeToTextInfo result;
		ThrowOSStatus ( ::CreateUnicodeToTextInfoByEncoding ( iEncoding, &result ));
		return Nucleus::Owned<UnicodeToTextInfo>::Seize ( result );
		}

	inline Nucleus::Owned<UnicodeToTextRunInfo> CreateUnicodeToTextRunInfo ( ItemCount iNumberOfMappings, const UnicodeMapping iUnicodeMappings[] ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		UnicodeToTextRunInfo result;
		ThrowOSStatus ( ::CreateUnicodeToTextRunInfo ( iNumberOfMappings, iUnicodeMappings, &result ));
		return Nucleus::Owned<UnicodeToTextRunInfo>::Seize ( result );
		}
	
	inline Nucleus::Owned<UnicodeToTextRunInfo> CreateUnicodeToTextRunInfoByEncoding ( ItemCount iNumberOfEncodings, const ::TextEncoding iEncodings[] ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		UnicodeToTextRunInfo result;
		ThrowOSStatus ( ::CreateUnicodeToTextRunInfoByEncoding ( iNumberOfEncodings, iEncodings, &result ));
		return Nucleus::Owned<UnicodeToTextRunInfo>::Seize ( result );
		}

	inline Nucleus::Owned<UnicodeToTextRunInfo> CreateUnicodeToTextRunInfoByScriptCode ( ItemCount iNumberOfScriptCodes, const ::ScriptCode iScripts[] ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		UnicodeToTextRunInfo result;
		ThrowOSStatus ( ::CreateUnicodeToTextRunInfoByScriptCode ( iNumberOfScriptCodes, iScripts, &result ));
		return Nucleus::Owned<UnicodeToTextRunInfo>::Seize ( result );
		}

//	I think that these two  should possibly return a new info, rather than changing the input
	inline void ChangeTextToUnicodeInfo ( TextToUnicodeInfo ioTextToUnicodeInfo, const UnicodeMapping &iUnicodeMapping ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::ChangeTextToUnicodeInfo ( ioTextToUnicodeInfo, &iUnicodeMapping ));
		}

	inline void ChangeUnicodeToTextInfo ( UnicodeToTextInfo ioUnicodeToTextInfo, const UnicodeMapping &iUnicodeMapping ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::ChangeUnicodeToTextInfo ( ioUnicodeToTextInfo, &iUnicodeMapping ));
		}

	inline void DisposeTextToUnicodeInfo ( TextToUnicodeInfo *ioTextToUnicodeInfo ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::DisposeTextToUnicodeInfo ( ioTextToUnicodeInfo ));
		}
	
	inline void DisposeUnicodeToTextInfo ( UnicodeToTextInfo *ioUnicodeToTextInfo ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::DisposeUnicodeToTextInfo ( ioUnicodeToTextInfo ));
		}
	
	inline void DisposeUnicodeToTextRunInfo ( UnicodeToTextRunInfo *ioUnicodeToTextRunInfo ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::DisposeUnicodeToTextRunInfo ( ioUnicodeToTextRunInfo ));
		}


//	Returns the length of the converted string (in Bytes)
	ByteCount ConvertFromTextToUnicode (
//	Inputs
			TextToUnicodeInfo iTextToUnicodeInfo, ByteCount iSourceLen, ConstLogicalAddress iSourceStr, OptionBits iControlFlags,
//	Offsets
			ItemCount iOffsetCount, const ByteOffset iOffsetArray[], ItemCount *oOffsetCount, ByteOffset oOffsetArray[],
//	Outputs
			ByteCount iOutputBufLen, ByteCount * oSourceRead, UniChar oUnicodeStr[] );
			
//	A simple case without the offsets
	ByteCount ConvertFromTextToUnicode (
//	Inputs
			TextToUnicodeInfo iTextToUnicodeInfo, ByteCount iSourceLen, ConstLogicalAddress iSourceStr, OptionBits iControlFlags,
//	Outputs
			ByteCount iOutputBufLen, ByteCount * oSourceRead, UniChar oUnicodeStr[] );


//	Returns the length of the converted string (in Bytes)

	ByteCount ConvertFromUnicodeToText ( 
//	Unicode inputs
			UnicodeToTextInfo iUnicodeToTextInfo, ByteCount iUnicodeLen, const UniChar iUnicodeStr[], OptionBits iControlFlags,
//	Offsets
			ItemCount iOffsetCount, const ByteOffset iOffsetArray[], ItemCount * oOffsetCount, ByteOffset oOffsetArray[],
//	Outputs
			ByteCount iOutputBufLen, ByteCount * oInputRead, LogicalAddress oOutputStr );

//	A simple case without the offsets			
	ByteCount ConvertFromUnicodeToText ( 
//	Unicode inputs
			UnicodeToTextInfo iUnicodeToTextInfo, ByteCount iUnicodeLen, const UniChar iUnicodeStr[], OptionBits iControlFlags,
//	Outputs
			ByteCount iOutputBufLen, ByteCount *oInputRead, LogicalAddress oOutputStr );


#if 0
/*
 *  ConvertFromUnicodeToTextRun()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
ConvertFromUnicodeToTextRun(
  UnicodeToTextRunInfo   iUnicodeToTextInfo,
  ByteCount              iUnicodeLen,
  const UniChar          iUnicodeStr[],
  OptionBits             iControlFlags,
  ItemCount              iOffsetCount,
  const ByteOffset       iOffsetArray[],           /* can be NULL */
  ItemCount *            oOffsetCount,             /* can be NULL */
  ByteOffset             oOffsetArray[],           /* can be NULL */
  ByteCount              iOutputBufLen,
  ByteCount *            oInputRead,
  ByteCount *            oOutputLen,
  LogicalAddress         oOutputStr,
  ItemCount              iEncodingRunBufLen,
  ItemCount *            oEncodingRunOutLen,
  TextEncodingRun        oEncodingRuns[])                     AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  ConvertFromUnicodeToScriptCodeRun()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
ConvertFromUnicodeToScriptCodeRun(
  UnicodeToTextRunInfo   iUnicodeToTextInfo,
  ByteCount              iUnicodeLen,
  const UniChar          iUnicodeStr[],
  OptionBits             iControlFlags,
  ItemCount              iOffsetCount,
  const ByteOffset       iOffsetArray[],           /* can be NULL */
  ItemCount *            oOffsetCount,             /* can be NULL */
  ByteOffset             oOffsetArray[],           /* can be NULL */
  ByteCount              iOutputBufLen,
  ByteCount *            oInputRead,
  ByteCount *            oOutputLen,
  LogicalAddress         oOutputStr,
  ItemCount              iScriptRunBufLen,
  ItemCount *            oScriptRunOutLen,
  ScriptCodeRun          oScriptCodeRuns[])                   AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/* Truncate a multibyte string at a safe place. */
/*
 *  TruncateForTextToUnicode()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
TruncateForTextToUnicode(
  ConstTextToUnicodeInfo   iTextToUnicodeInfo,
  ByteCount                iSourceLen,
  ConstLogicalAddress      iSourceStr,
  ByteCount                iMaxLen,
  ByteCount *              oTruncatedLen)                     AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/* Truncate a Unicode string at a safe place. */
/*
 *  TruncateForUnicodeToText()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
TruncateForUnicodeToText(
  ConstUnicodeToTextInfo   iUnicodeToTextInfo,
  ByteCount                iSourceLen,
  const UniChar            iSourceStr[],
  OptionBits               iControlFlags,
  ByteCount                iMaxLen,
  ByteCount *              oTruncatedLen)                     AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/* Convert a Pascal string to Unicode string. */
/*
 *  ConvertFromPStringToUnicode()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
ConvertFromPStringToUnicode(
  TextToUnicodeInfo   iTextToUnicodeInfo,
  ConstStr255Param    iPascalStr,
  ByteCount           iOutputBufLen,
  ByteCount *         oUnicodeLen,
  UniChar             oUnicodeStr[])                          AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/* Convert a Unicode string to Pascal string. */
/*
 *  ConvertFromUnicodeToPString()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
ConvertFromUnicodeToPString(
  UnicodeToTextInfo   iUnicodeToTextInfo,
  ByteCount           iUnicodeLen,
  const UniChar       iUnicodeStr[],
  Str255              oPascalStr)                             AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif


	inline ItemCount CountUnicodeMappings ( OptionBits iFilter, ConstUnicodeMappingPtr iFindMapping ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		::ItemCount result;
		ThrowOSStatus ( ::CountUnicodeMappings ( iFilter, iFindMapping, &result ));
		return result;
		}
	
#if 0
/* Get a list of the available conversion mappings. */
/*
 *  QueryUnicodeMappings()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
QueryUnicodeMappings(
  OptionBits               iFilter,
  ConstUnicodeMappingPtr   iFindMapping,
  ItemCount                iMaxCount,
  ItemCount *              oActualCount,
  UnicodeMapping           oReturnedMappings[])               AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif


#if 0
/* Setup the fallback handler for converting Unicode To Text. */
/*
 *  SetFallbackUnicodeToText()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
SetFallbackUnicodeToText(
  UnicodeToTextInfo          iUnicodeToTextInfo,
  UnicodeToTextFallbackUPP   iFallback,
  OptionBits                 iControlFlags,
  LogicalAddress             iInfoPtr)                        AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/* Setup the fallback handler for converting Unicode To TextRuns. */
/*
 *  SetFallbackUnicodeToTextRun()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in UnicodeConverter 1.1 and later
 */
extern OSStatus 
SetFallbackUnicodeToTextRun(
  UnicodeToTextRunInfo       iUnicodeToTextRunInfo,
  UnicodeToTextFallbackUPP   iFallback,
  OptionBits                 iControlFlags,
  LogicalAddress             iInfoPtr)                        AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif


	inline void ResetTextToUnicodeInfo ( TextToUnicodeInfo ioTextToUnicodeInfo ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::ResetTextToUnicodeInfo ( ioTextToUnicodeInfo ));
		}
	
	inline void ResetUnicodeToTextInfo ( UnicodeToTextInfo ioUnicodeToTextInfo ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::ResetUnicodeToTextInfo ( ioUnicodeToTextInfo ));
		}

	inline void ResetUnicodeToTextRunInfo ( UnicodeToTextRunInfo ioUnicodeToTextRunInfo ) {
		(void) TextEncodingConversionManagerErrorsRegistrationDependency();
		ThrowOSStatus ( ::ResetUnicodeToTextRunInfo ( ioUnicodeToTextRunInfo ));
		}

  	}

#endif
