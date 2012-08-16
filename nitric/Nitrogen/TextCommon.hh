// Nitrogen/TextCommon.hh
// ----------------------

// Part of the Nitrogen project.
//
// Written 2004-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_TEXTCOMMON_HH
#define NITROGEN_TEXTCOMMON_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif
#ifndef NUCLEUS_STRING_HH
#include "nucleus/string.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_LANGCODE_HH
#include "Mac/Script/Types/LangCode.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_REGIONCODE_HH
#include "Mac/Script/Types/RegionCode.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_SCRIPTCODE_HH
#include "Mac/Script/Types/ScriptCode.hh"
#endif


namespace Nitrogen
  {
  
	class TextEncodingConversionManagerErrorsRegistrationDependency
	{
		public:
			TextEncodingConversionManagerErrorsRegistrationDependency();
	};
	
	enum TextEncodingBase
	{
		kTextEncodingBase_Max = nucleus::enumeration_traits< ::TextEncodingBase >::max
	};
	
	enum TextEncodingVariant
	{
		kTextEncodingDefaultVariant = ::kTextEncodingDefaultVariant,
		
		kTextEncodingVariant_Max = nucleus::enumeration_traits< ::TextEncodingVariant >::max
	};
	
	enum TextEncodingFormat
	{
		kTextEncodingFormat_Max = nucleus::enumeration_traits< ::TextEncodingFormat >::max
	};
	
	enum TextEncoding
	{
		kTextEncodingUnknown = ::kTextEncodingUnknown,
		
		kTextEncoding_Max = nucleus::enumeration_traits< ::TextEncoding >::max
	};
	
	enum TextEncodingNameSelector
	{
		kTextEncodingNameSelector_Max = nucleus::enumeration_traits< ::TextEncodingNameSelector >::max
	};
	
  }

namespace nucleus
  {
   template <>
   struct maker< TextEncodingRun >
     {
      TextEncodingRun operator()( ByteOffset offset, TextEncoding textEncoding ) const
        {
         TextEncodingRun result;
         result.offset = offset;
         result.textEncoding = textEncoding;
         return result;
        }
     };

   template <>
   struct maker< ScriptCodeRun >
     {
      ScriptCodeRun operator()( ByteOffset offset, Mac::ScriptCode script ) const
        {
         ScriptCodeRun result;
         result.offset = offset;
         result.script = script;
         return result;
        }
     };
  }

namespace Nitrogen
  {
	enum UCCharPropertyType
	{
		kUCCharPropertyType_Max = nucleus::enumeration_traits< ::UCCharPropertyType >::max
	};
	
	enum UCCharPropertyValue
	{
		kUCCharPropertyValue_Max = nucleus::enumeration_traits< ::UCCharPropertyValue >::max
	};
	
   // UCCharPropertyValue is really three types overloaded: UCCharGenlCategory, UCCharCombiningClass, and UCCharBidiCategory.
   
	enum UCCharGenlCategory
	{
		kUCCharGenlCategory_Max = nucleus::enumeration_traits< ::UCCharPropertyValue >::max
	};
	
	enum UCCharCombiningClass
	{
		kUCCharCombiningClass_Max = nucleus::enumeration_traits< ::UCCharPropertyValue >::max
	};
	  
	enum UCCharBidiCategory
	{
		kUCCharBidiCategory_Max = nucleus::enumeration_traits< ::UCCharPropertyValue >::max
	};
	

   /* TECInfo... */
   
   inline TextEncoding CreateTextEncoding( TextEncodingBase encodingBase,
                                           TextEncodingVariant encodingVariant,
                                           TextEncodingFormat encodingFormat )
     {
      return TextEncoding( ::CreateTextEncoding( encodingBase, encodingVariant, encodingFormat ) );
     }
   
   inline TextEncodingBase GetTextEncodingBase( TextEncoding encoding )
     {
      return TextEncodingBase( ::GetTextEncodingBase( encoding ) );
     }
   
   inline TextEncodingVariant GetTextEncodingVariant( TextEncoding encoding )
     {
      return TextEncodingVariant( ::GetTextEncodingVariant( encoding ) );
     }
   
   inline TextEncodingFormat GetTextEncodingFormat( TextEncoding encoding )
     {
      return TextEncodingFormat( ::GetTextEncodingFormat( encoding ) );
     }
   
   inline TextEncoding ResolveDefaultTextEncoding( TextEncoding encoding )
     {
      return TextEncoding( ::ResolveDefaultTextEncoding( encoding ) );
     }
   
   struct GetTextEncodingName_Result
     {
      ByteCount        oNameLength;
      Mac::RegionCode  oActualRegion;
      TextEncoding     oActualEncoding;
     };
   
   GetTextEncodingName_Result GetTextEncodingName( TextEncoding             iEncoding,
                                                   TextEncodingNameSelector iNamePartSelector,
                                                   Mac::RegionCode          iPreferredRegion,
                                                   TextEncoding             iPreferredEncoding,
                                                   ByteCount                iOutputBufLen,
                                                   UInt8                   *oEncodingName );
   
   template <ByteCount iOutputBufLen>
   inline GetTextEncodingName_Result GetTextEncodingName( TextEncoding             iEncoding,
                                                          TextEncodingNameSelector iNamePartSelector,
                                                          Mac::RegionCode          iPreferredRegion,
                                                          TextEncoding             iPreferredEncoding,
                                                          UInt8                    (&oEncodingName)[iOutputBufLen] )
     {
      return GetTextEncoding( iEncoding, iNamePartSelector, iPreferredRegion, iPreferredEncoding, iOutputBufLen, oEncodingName );
     }
   
   struct GetTextEncodingName_ResultWithString
     {
      nucleus::mutable_string  oEncodingName;
      Mac::RegionCode          oActualRegion;
      TextEncoding             oActualEncoding;
      
      operator const nucleus::string&() const    { return oEncodingName; }
     };

   GetTextEncodingName_ResultWithString GetTextEncodingName( TextEncoding             iEncoding,
                                                             TextEncodingNameSelector iNamePartSelector,
                                                             Mac::RegionCode          iPreferredRegion,
                                                             TextEncoding             iPreferredEncoding );

   nucleus::owned< TECInfoHandle, nucleus::disposer< Mac::Handle > > TECGetInfo();

   TextEncoding UpgradeScriptInfoToTextEncoding( Mac::ScriptCode  iTextScriptID,
                                                 Mac::LangCode    iTextLanguageID = Mac::kTextLanguageDontCare,
                                                 Mac::RegionCode  iRegionID       = Mac::kTextRegionDontCare,
                                                 ConstStr255Param iTextFontname   = 0 );

	inline TextEncoding UpgradeScriptInfoToTextEncoding( Mac::ScriptCode   iTextScriptID,
	                                                     Mac::RegionCode   iRegionID,
	                                                     ConstStr255Param  iTextFontname = 0 )
	{
		return UpgradeScriptInfoToTextEncoding( iTextScriptID,
		                                        Mac::kTextLanguageDontCare,
		                                        iRegionID,
		                                        iTextFontname );
	}
	
	inline TextEncoding UpgradeScriptInfoToTextEncoding( Mac::ScriptCode   iTextScriptID,
	                                                     Mac::LangCode     iTextLanguageID,
	                                                     ConstStr255Param  iTextFontname )
	{
		return UpgradeScriptInfoToTextEncoding( iTextScriptID,
		                                        iTextLanguageID,
		                                        Mac::kTextRegionDontCare,
		                                        iTextFontname );
	}
	
	inline TextEncoding UpgradeScriptInfoToTextEncoding( Mac::ScriptCode   iTextScriptID,
	                                                     ConstStr255Param  iTextFontname )
	{
		return UpgradeScriptInfoToTextEncoding( iTextScriptID,
		                                        Mac::kTextLanguageDontCare,
		                                        Mac::kTextRegionDontCare,
		                                        iTextFontname );
	}
	
	inline TextEncoding UpgradeScriptInfoToTextEncoding( Mac::LangCode     iTextLanguageID,
	                                                     Mac::RegionCode   iRegionID       = Mac::kTextRegionDontCare,
	                                                     ConstStr255Param  iTextFontname   = 0 )
	{
		return UpgradeScriptInfoToTextEncoding( Mac::kTextScriptDontCare,
		                                        iTextLanguageID,
		                                        iRegionID,
		                                        iTextFontname );
	}
	
	inline TextEncoding UpgradeScriptInfoToTextEncoding( Mac::LangCode     iTextLanguageID,
	                                                     ConstStr255Param  iTextFontname )
	{
		return UpgradeScriptInfoToTextEncoding( Mac::kTextScriptDontCare,
		                                        iTextLanguageID,
		                                        Mac::kTextRegionDontCare,
		                                        iTextFontname );
	}
	
	inline TextEncoding UpgradeScriptInfoToTextEncoding( Mac::RegionCode   iRegionID,
	                                                     ConstStr255Param  iTextFontname = 0 )
	{
		return UpgradeScriptInfoToTextEncoding( Mac::kTextScriptDontCare,
		                                        Mac::kTextLanguageDontCare,
		                                        iRegionID,
		                                        iTextFontname );
	}
	
	inline TextEncoding UpgradeScriptInfoToTextEncoding( ConstStr255Param iTextFontname )
	{
		return UpgradeScriptInfoToTextEncoding( Mac::kTextScriptDontCare,
		                                        Mac::kTextLanguageDontCare,
		                                        Mac::kTextRegionDontCare,
		                                        iTextFontname );
	}

   struct RevertTextEncodingToScriptInfo_Result
     {
      Mac::ScriptCode  oTextScriptID;
      Mac::LangCode    oTextLanguageID;
      Str255           oTextFontname;
     };
   
   RevertTextEncodingToScriptInfo_Result RevertTextEncodingToScriptInfo( TextEncoding iEncoding );

   struct NearestMacTextEncodings_Result
     {
      TextEncoding bestMacEncoding;
      TextEncoding alternateMacEncoding;
     };

   NearestMacTextEncodings_Result NearestMacTextEncodings( TextEncoding generalEncoding );

   UCCharPropertyValue UCGetCharProperty( const UniChar     *charPtr,
                                          UniCharCount       textLength,
                                          UCCharPropertyType propType );

   template < class UniString >
   UCCharPropertyValue UCGetCharProperty( const UniString&   text,
                                          UCCharPropertyType propType )
	{
		return UCGetCharProperty( text.data(),
		                          text.size(),
		                          propType );
	}
   
   template < ::UCCharPropertyType propType >
   struct UCGetCharProperty_Traits;

   template <> struct UCGetCharProperty_Traits< kUCCharPropTypeGenlCategory   > { typedef UCCharGenlCategory   Result; };
   template <> struct UCGetCharProperty_Traits< kUCCharPropTypeCombiningClass > { typedef UCCharCombiningClass Result; };
   template <> struct UCGetCharProperty_Traits< kUCCharPropTypeBidiCategory   > { typedef UCCharBidiCategory   Result; };
   
   template < ::UCCharPropertyType propType >
   typename UCGetCharProperty_Traits<propType>::Result UCGetCharProperty( const UniChar *charPtr,
                                                                          UniCharCount   textLength )
     {
      return UCGetCharProperty( charPtr, textLength, propType );
     }
   
   template < ::UCCharPropertyType propType, class UniString >
   typename UCGetCharProperty_Traits<propType>::Result UCGetCharProperty( const UniString& text )
     {
      return UCGetCharProperty( text, propType );
     }
  }

#endif
