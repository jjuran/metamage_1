/*
	Mac/AppleEvents/Types/AEKeyword_scribe.hh
	-----------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBE_HH
#define MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBE_HH

// iota
#ifndef IOTA_NULLFILTER_HH
#include "iota/null_filter.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AECOMPOPERATOR_HH
#include "Mac/AppleEvents/Types/AECompOperator.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEEVENTCLASS_HH
#include "Mac/AppleEvents/Types/AEEventClass.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEEVENTID_HH
#include "Mac/AppleEvents/Types/AEEventID.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYFORM_HH
#include "Mac/AppleEvents/Types/AEKeyForm.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AELOGICALOPERATOR_HH
#include "Mac/AppleEvents/Types/AELogicalOperator.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEOBJECTCLASS_HH
#include "Mac/AppleEvents/Types/AEObjectClass.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AERETURNID_HH
#include "Mac/AppleEvents/Types/AEReturnID.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AETRANSACTIONID_HH
#include "Mac/AppleEvents/Types/AETransactionID.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH
#include "Mac/AppleEvents/Types/DescType_scribe.hh"
#endif


namespace Mac
{
	
	// The following concern AEKeywords which always use the same DescType.
	
	// A keyword whose DescType has a scribe for the keyword's C++ type may
	// specialize AEKeyword_DescType_simple to specify the DescType.  Those
	// keywords with ambiguous DescTypes (such as typeType) must specialize
	// AEKeyword_DescType and either AEKeyword_data (for converting POD) or
	// AEKeyword_scribe for a custom scribe.
	
	// Specialize for keywords with unambiguous DescTypes.
	// For others, default to a null value.
	template < AEKeyword > struct AEKeyword_DescType_simple : DescType_< DescType( 0 ) > {};
	
	// Default to a DescType mapped above, but filter out zero so failure
	// to specialize AEKeyword_DescType is an error.
	template < AEKeyword key > struct AEKeyword_DescType : iota::null_filter< AEKeyword_DescType_simple< key > > {};
	
	// Specialize to name the C++ type for a keyword if AEKeyword_DescType
	// is also specialized.
	template < AEKeyword > struct AEKeyword_data;
	
	// If the DescType is unique 
	template < AEKeyword key, DescType unique_DescType >
	struct AEKeyword_scribe_impl : DescType_scribe< unique_DescType >
	{
	};
	
	template < AEKeyword key >
	struct AEKeyword_scribe_impl< key, DescType( 0 ) >
	{
		static const DescType descType = AEKeyword_DescType< key >::value;
		
		typedef typename DescType_integer< descType >::type integer_type;
		
		typedef typename AEKeyword_data< key >::type data_type;
		
		typedef nucleus::converting_POD_scribe< data_type, integer_type > type;
	};
	
	template < AEKeyword key >
	struct AEKeyword_scribe : AEKeyword_scribe_impl< key, AEKeyword_DescType_simple< key >::value >
	{
	};
	
	
	template < Mac::AEKeyword keyword >
	struct AEKeyword_parameter
	{
		typedef typename Mac::AEKeyword_scribe< keyword >::type::argument_type type;
	};
	
	template < Mac::AEKeyword keyword >
	struct AEKeyword_result
	{
		typedef typename Mac::AEKeyword_scribe< keyword >::type::result_type type;
	};
	
	template < AEKeyword keyword, class Putter >
	inline void AEKeyword_put( typename AEKeyword_parameter< keyword >::type param, const Putter& putter )
	{
		AEKeyword_scribe< keyword >::type::Put( param, putter );
	}
	
	template < AEKeyword keyword, class Getter >
	inline typename AEKeyword_result< keyword >::type
	//
	AEKeyword_get( const Getter& getter )
	{
		return AEKeyword_scribe< keyword >::type::Get( getter );
	}
	
	
	// AEDataModel
	// -----------
	
	template <> struct AEKeyword_DescType< keyTransactionIDAttr > : Integer_DescType< ::AETransactionID > {};
	template <> struct AEKeyword_DescType< keyReturnIDAttr      > : Integer_DescType< ::SInt32          > {};
	
	template <> struct AEKeyword_DescType< keyEventClassAttr > : DescType_< typeType > {};
	template <> struct AEKeyword_DescType< keyEventIDAttr    > : DescType_< typeType > {};
	
	template <> struct AEKeyword_DescType< keyEventSourceAttr > : Integer_DescType< SInt16 > {};
	
	template <> struct AEKeyword_data< keyTransactionIDAttr > : type_< AETransactionID  > {};
	template <> struct AEKeyword_data< keyReturnIDAttr      > : type_< AEReturnID_32Bit > {};
	
	template <> struct AEKeyword_data< keyEventClassAttr > : type_< AEEventClass > {};
	template <> struct AEKeyword_data< keyEventIDAttr    > : type_< AEEventID    > {};
	
	template <> struct AEKeyword_data< keyEventSourceAttr > : type_< AEEventSource > {};
	
	template <> struct AEKeyword_DescType_simple< keyMissedKeywordAttr > : DescType_< typeKeyword > {};
	
	// AEObjects
	// ---------
	
	template <> struct AEKeyword_DescType< keyAECompOperator > : DescType_< typeType > {};
	
	template <> struct AEKeyword_DescType< keyAELogicalOperator > : DescType_< typeType > {};
	
	template <> struct AEKeyword_DescType< keyAEDesiredClass > : DescType_< typeType > {};
	
	template <> struct AEKeyword_DescType< keyAEKeyForm > : DescType_< typeEnumerated > {};
	
	template <> struct AEKeyword_data< keyAECompOperator    > : type_< AECompOperator    > {};
	template <> struct AEKeyword_data< keyAELogicalOperator > : type_< AELogicalOperator > {};
	template <> struct AEKeyword_data< keyAEDesiredClass    > : type_< AEObjectClass     > {};
	template <> struct AEKeyword_data< keyAEKeyForm         > : type_< AEKeyForm         > {};
	
	// AERegistry
	// ----------
	
	template <> struct AEKeyword_DescType< keyAEObjectClass > : DescType_< typeType > {};
	
	template <> struct AEKeyword_data< keyAEObjectClass > : type_< AEObjectClass > {};
	
	// AppleEvents
	// -----------
	
	template <> struct AEKeyword_DescType< keyErrorNumber > : Integer_DescType< ::OSErr > {};
	
	template <> struct AEKeyword_DescType< keyErrorString > : DescType_< typeText > {};
	
	template <> struct AEKeyword_data< keyErrorNumber > : type_< AEEventSource > {};
	
	template <> struct AEKeyword_DescType_simple< keyProcessSerialNumber > : DescType_< typeProcessSerialNumber > {};
	
}

#endif

