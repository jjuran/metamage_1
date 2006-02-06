// TextCommon.cp

#ifndef NITROGEN_TEXTCOMMON_H
#include "Nitrogen/TextCommon.h"
#endif
#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
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
      Nucleus::OnlyOnce<RegisterTextEncodingConversionManagerErrors>();

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

      result.oActualRegion   = RegionCode  ( oActualRegion   );
      result.oActualEncoding = TextEncoding( oActualEncoding );

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

   Nucleus::Owned< TECInfoHandle, Nucleus::Disposer<Handle> > TECGetInfo()
     {
      Nucleus::OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      ::TECInfoHandle result;
      ThrowOSStatus( ::TECGetInfo( &result ) );
      return Nucleus::Owned< TECInfoHandle, Nucleus::Disposer<Handle> >::Seize( result );
     }
   
   TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode iTextScriptID,
                                                 LangCode iTextLanguageID,
                                                 RegionCode iRegionID,
                                                 ConstStr255Param iTextFontname )
     {
      Nucleus::OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      ::TextEncoding result;
      ThrowOSStatus( ::UpgradeScriptInfoToTextEncoding( iTextScriptID,
                                                        iTextLanguageID,
                                                        iRegionID,
                                                        iTextFontname,
                                                        &result ) );
      return TextEncoding( result );
     }

   RevertTextEncodingToScriptInfo_Result RevertTextEncodingToScriptInfo( TextEncoding iEncoding )
     {
      Nucleus::OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      RevertTextEncodingToScriptInfo_Result result;
      ::ScriptCode oTextScriptID;
      ::LangCode   oTextLanguageID;
      ThrowOSStatus( ::RevertTextEncodingToScriptInfo( iEncoding,
                                                       &oTextScriptID,
                                                       &oTextLanguageID,
                                                       result.oTextFontname ) );
      result.oTextScriptID   = ScriptCode( oTextScriptID );
      result.oTextLanguageID = LangCode( oTextLanguageID );
      return result;
     }

   NearestMacTextEncodings_Result NearestMacTextEncodings( TextEncoding generalEncoding )
     {
      Nucleus::OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      
      ::TextEncoding bestMacEncoding;
      ::TextEncoding alternateMacEncoding;
      ThrowOSStatus( ::NearestMacTextEncodings( generalEncoding, &bestMacEncoding, &alternateMacEncoding ) );

      NearestMacTextEncodings_Result result;
      result.bestMacEncoding      = TextEncoding( bestMacEncoding      );
      result.alternateMacEncoding = TextEncoding( alternateMacEncoding );

      return result;
     }

   UCCharPropertyValue UCGetCharProperty( const UniChar     *charPtr,
                                          UniCharCount       textLength,
                                          UCCharPropertyType propType )
     {
      Nucleus::OnlyOnce<RegisterTextEncodingConversionManagerErrors>();
      ::UCCharPropertyValue result;
      ThrowOSStatus( ::UCGetCharProperty( charPtr, textLength, propType, &result ) );
      return UCCharPropertyValue( result );
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
