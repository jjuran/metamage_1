// TextCommon.cp

#ifndef NITROGEN_TEXTCOMMON_H
#include "Nitrogen/TextCommon.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   GetTextEncodingName_Result GetTextEncodingName( TextEncoding             iEncoding,
                                                   TextEncodingNameSelector iNamePartSelector,
                                                   RegionCode               iPreferredRegion,
                                                   TextEncoding             iPreferredEncoding,
                                                   ByteCount                iOutputBufLen,
                                                   UInt8                   *oEncodingName )
     {
      OnlyOnce<RegisterTextEncodingConversionManagerErrors>();

      GetTextEncodingName_Result result;
      ::RegionCode oActualRegion;
      ::TextEncoding oActualEncoding;

      ThrowOSStatus( ::GetTextEncodingName( iEncoding,
                                            iNamePartSelector,
                                            iPreferredRegion,
                                            iPreferredEncoding,
                                            iOutputBufLen,
                                            &result.oNameLength,
                                            &oActualRegion,
                                            &oActualEncoding,
                                            oEncodingName ) );

      result.oActualRegion = oActualRegion;
      result.oActualEncoding = oActualEncoding;

      return result;
     }
   

   GetTextEncodingName_ResultWithString GetTextEncodingName( TextEncoding             iEncoding,
                                                             TextEncodingNameSelector iNamePartSelector,
                                                             RegionCode               iPreferredRegion,
                                                             TextEncoding             iPreferredEncoding )
     {
      GetTextEncodingName_ResultWithString result;
      result.oEncodingName.resize( 256 );
      
      GetTextEncodingName_Result attempt = GetTextEncodingName( iEncoding,
                                                                iNamePartSelector,
                                                                iPreferredRegion,
                                                                iPreferredEncoding,
                                                                result.oEncodingName.size(),
                                                                reinterpret_cast<UInt8*>( &*result.oEncodingName.begin() ) );
         
      while ( attempt.oNameLength > result.oEncodingName.size() )
        {
         result.oEncodingName.resize( attempt.oNameLength );
         attempt = GetTextEncodingName( iEncoding,
                                        iNamePartSelector,
                                        iPreferredRegion,
                                        iPreferredEncoding,
                                        result.oEncodingName.size(),
                                        reinterpret_cast<UInt8*>( &*result.oEncodingName.begin() ) );
        }

      result.oEncodingName.resize( attempt.oNameLength );
      result.oActualRegion   = attempt.oActualRegion;
      result.oActualEncoding = attempt.oActualEncoding;
      
      return result;
     }

   Owned< TECInfoHandle, Disposer<Handle> > TECGetInfo()
     {
      OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      ::TECInfoHandle result;
      ThrowOSStatus( ::TECGetInfo( &result ) );
      return Owned< TECInfoHandle, Disposer<Handle> >::Seize( result );
     }
   
   TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode iTextScriptID,
                                                 LangCode iTextLanguageID,
                                                 RegionCode iRegionID,
                                                 ConstStr255Param iTextFontname )
     {
      OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      ::TextEncoding result;
      ThrowOSStatus( ::UpgradeScriptInfoToTextEncoding( iTextScriptID,
                                                        iTextLanguageID,
                                                        iRegionID,
                                                        iTextFontname,
                                                        &result ) );
      return result;
     }

   RevertTextEncodingToScriptInfo_Result RevertTextEncodingToScriptInfo( TextEncoding iEncoding )
     {
      OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      RevertTextEncodingToScriptInfo_Result result;
      ::ScriptCode oTextScriptID;
      ::LangCode   oTextLanguageID;
      ThrowOSStatus( ::RevertTextEncodingToScriptInfo( iEncoding,
                                                       &oTextScriptID,
                                                       &oTextLanguageID,
                                                       result.oTextFontname ) );
      result.oTextScriptID = oTextScriptID;
      result.oTextLanguageID = oTextLanguageID;
      return result;
     }

   NearestMacTextEncodings_Result NearestMacTextEncodings( TextEncoding generalEncoding )
     {
      OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      
      ::TextEncoding bestMacEncoding;
      ::TextEncoding alternateMacEncoding;
      ThrowOSStatus( ::NearestMacTextEncodings( generalEncoding, &bestMacEncoding, &alternateMacEncoding ) );

      NearestMacTextEncodings_Result result;
      result.bestMacEncoding      = bestMacEncoding;
      result.alternateMacEncoding = alternateMacEncoding;

      return result;
     }

   UCCharPropertyValue UCGetCharProperty( const UniChar     *charPtr,
                                          UniCharCount       textLength,
                                          UCCharPropertyType propType )
     {
      OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      ::UCCharPropertyValue result;
      ThrowOSStatus( ::UCGetCharProperty( charPtr, textLength, propType, &result ) );
      return result;
     }

   UCCharPropertyValue UCGetCharProperty( const UniString&   text,
                                          UCCharPropertyType propType )
     {
      return UCGetCharProperty( text.empty() ? 0 : &*text.begin(),
                                text.size(),
                                propType );
     }

   void RegisterTextEncodingConversionManagerErrors()
     {
      RegisterOSStatus< kTextUnsupportedEncodingErr   >();
      RegisterOSStatus< kTextMalformedInputErr        >();
      RegisterOSStatus< kTextUndefinedElementErr      >();
      RegisterOSStatus< kTECMissingTableErr           >();
      RegisterOSStatus< kTECTableChecksumErr          >();
      RegisterOSStatus< kTECTableFormatErr            >();
      RegisterOSStatus< kTECCorruptConverterErr       >();
      RegisterOSStatus< kTECNoConversionPathErr       >();
      RegisterOSStatus< kTECBufferBelowMinimumSizeErr >();
      RegisterOSStatus< kTECArrayFullErr              >();
      RegisterOSStatus< kTECBadTextRunErr             >();
      RegisterOSStatus< kTECPartialCharErr            >();
      RegisterOSStatus< kTECUnmappableElementErr      >();
      RegisterOSStatus< kTECIncompleteElementErr      >();
      RegisterOSStatus< kTECDirectionErr              >();
      RegisterOSStatus< kTECGlobalsUnavailableErr     >();
      RegisterOSStatus< kTECItemUnavailableErr        >();
      RegisterOSStatus< kTECUsedFallbacksStatus       >();
      RegisterOSStatus< kTECNeedFlushStatus           >();
      RegisterOSStatus< kTECOutputBufferFullStatus    >();
      RegisterOSStatus< unicodeChecksumErr            >();
      RegisterOSStatus< unicodeNoTableErr             >();
      RegisterOSStatus< unicodeVariantErr             >();
      RegisterOSStatus< unicodeFallbacksErr           >();
      RegisterOSStatus< unicodePartConvertErr         >();
      RegisterOSStatus< unicodeBufErr                 >();
      RegisterOSStatus< unicodeCharErr                >();
      RegisterOSStatus< unicodeElementErr             >();
      RegisterOSStatus< unicodeNotFoundErr            >();
      RegisterOSStatus< unicodeTableFormatErr         >();
      RegisterOSStatus< unicodeDirectionErr           >();
      RegisterOSStatus< unicodeContextualErr          >();
      RegisterOSStatus< unicodeTextEncodingDataErr    >();
     }
  }
