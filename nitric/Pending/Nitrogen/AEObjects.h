// AEObjects.h

#ifndef NITROGEN_AEOBJECTS_H
#define NITROGEN_AEOBJECTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEOBJECTS__
#include FRAMEWORK_HEADER(AE,AEObjects.h)
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif

namespace Nitrogen
{
	
	void RegisterObjectSupportLibraryErrors();
	
	#pragma mark -
	#pragma mark ¥ Constant types ¥
	
	class AELogicalOperator_Tag {};
	typedef SelectorType< AELogicalOperator_Tag, ::FourCharCode > AELogicalOperator;
	typedef AELogicalOperator AELogicalDescriptor;
	
	class AEAbsoluteOrdinal_Tag {};
	typedef SelectorType< AEAbsoluteOrdinal_Tag, ::FourCharCode > AEAbsoluteOrdinal;
	
	class AERelativeOrdinal_Tag {};
	typedef SelectorType< AERelativeOrdinal_Tag, ::FourCharCode > AERelativeOrdinal;
	typedef AERelativeOrdinal AERelativeDescriptor;
	
	struct AEResolveCallbackFlags_Tag  {};
	typedef FlagType< AEResolveCallbackFlags_Tag, short, kAEIDoMinimum > AEResolveCallbackFlags;
	
	
	#pragma mark -
	#pragma mark ¥ Constants ¥
	
	// *** LOGICAL OPERATOR CONSTANTS  ***
	static AELogicalOperator sAEAND      = kAEAND;
	static AELogicalOperator sAEOR       = kAEOR;
	static AELogicalOperator sAENOT      = kAENOT;
	
	// *** ABSOLUTE ORDINAL CONSTANTS  ***
	static AEAbsoluteOrdinal sAEFirst    = kAEFirst;
	static AEAbsoluteOrdinal sAELast     = kAELast;
	static AEAbsoluteOrdinal sAEMiddle   = kAEMiddle;
	static AEAbsoluteOrdinal sAEAny      = kAEAny;
	static AEAbsoluteOrdinal sAEAll      = kAEAll;
	
	// *** RELATIVE ORDINAL CONSTANTS  ***
	static AERelativeOrdinal sAENext     = kAENext;
	static AERelativeOrdinal sAEPrevious = kAEPrevious;
	
	
	// *** KEYWORD CONSTANT    ***
	static AEKeyword sKeyAECompOperator    = keyAECompOperator;
	static AEKeyword sKeyAELogicalTerms    = keyAELogicalTerms;
	static AEKeyword sKeyAELogicalOperator = keyAELogicalOperator;
	static AEKeyword sKeyAEObject1         = keyAEObject1;
	static AEKeyword sKeyAEObject2         = keyAEObject2;
	
	//     ... for Keywords for getting fields out of object specifier records. 
	static AEKeyword sKeyAEDesiredClass    = keyAEDesiredClass;
	static AEKeyword sKeyAEContainer       = keyAEContainer;
	static AEKeyword sKeyAEKeyForm         = keyAEKeyForm;
	static AEKeyword sKeyAEKeyData         = keyAEKeyData;

	//     ... for Keywords for getting fields out of Range specifier records. 
	static AEKeyword sKeyAERangeStart      = keyAERangeStart;
	static AEKeyword sKeyAERangeStop       = keyAERangeStop;
	
	//     ... special handler selectors for OSL Callbacks. 
	static AEKeyword sKeyDisposeTokenProc  = keyDisposeTokenProc;
	static AEKeyword sKeyAECompareProc     = keyAECompareProc;
	static AEKeyword sKeyAECountProc       = keyAECountProc;
	static AEKeyword sKeyAEMarkTokenProc   = keyAEMarkTokenProc;
	static AEKeyword sKeyAEMarkProc        = keyAEMarkProc;
	static AEKeyword sKeyAEAdjustMarksProc = keyAEAdjustMarksProc;
	static AEKeyword sKeyAEGetErrDescProc  = keyAEGetErrDescProc;

// ***   VALUE and TYPE CONSTANTS    ***
	
	//     ... possible values for the keyAEKeyForm field of an object specifier.
	static AEEnumerated sFormAbsolutePosition = formAbsolutePosition;
	static AEEnumerated sFormRelativePosition = formRelativePosition;
	static AEEnumerated sFormTest             = formTest;
	static AEEnumerated sFormRange            = formRange;
	static AEEnumerated sFormPropertyID       = formPropertyID;
	static AEEnumerated sFormName             = formName;
	
	//     ... relevant types (some of these are often pared with forms above).
	static DescType sTypeObjectSpecifier     = typeObjectSpecifier;
	static DescType sTypeObjectBeingExamined = typeObjectBeingExamined;
	static DescType sTypeCurrentContainer    = typeCurrentContainer;
	static DescType sTypeToken               = typeToken;
	static DescType sTypeRelativeDescriptor  = typeRelativeDescriptor;
	static DescType sTypeAbsoluteOrdinal     = typeAbsoluteOrdinal;
	static DescType sTypeIndexDescriptor     = typeIndexDescriptor;
	static DescType sTypeRangeDescriptor     = typeRangeDescriptor;
	static DescType sTypeLogicalDescriptor   = typeLogicalDescriptor;
	static DescType sTypeCompDescriptor      = typeCompDescriptor;
	static DescType sTypeOSLTokenList        = typeOSLTokenList;

//  Possible values for flags parameter to AEResolve.  They're additive
	static AEResolveCallbackFlags sAEIDoMinimum           = kAEIDoMinimum;
	static AEResolveCallbackFlags sAEIDoWhose             = kAEIDoWhose;
	static AEResolveCallbackFlags sAEIDoMarking           = kAEIDoMarking;
	static AEResolveCallbackFlags sAEPassSubDescs         = kAEPassSubDescs;
	static AEResolveCallbackFlags sAEResolveNestedLists   = kAEResolveNestedLists;
	static AEResolveCallbackFlags sAEHandleSimpleRanges   = kAEHandleSimpleRanges;
	static AEResolveCallbackFlags sAEUseRelativeIterators = kAEUseRelativeIterators;

// *** SPECIAL CONSTANTS FOR CUSTOM WHOSE-CLAUSE RESOLUTION
	
	static DescType sTypeWhoseDescriptor     = typeWhoseDescriptor;
	
	static AEEnumerated sFormWhose            = formWhose;
	
	static DescType sTypeWhoseRange          = typeWhoseRange;
	
	static AEKeyword sKeyAEWhoseRangeStart = keyAEWhoseRangeStart;
	static AEKeyword sKeyAEWhoseRangeStop  = keyAEWhoseRangeStop;
	static AEKeyword sKeyAEIndex           = keyAEIndex;
	static AEKeyword sKeyAETest            = keyAETest;
	
	
	#pragma mark -
	#pragma mark ¥ AEToken ¥
	
	struct TokenDisposer : public std::unary_function< AEToken, void >
	{
		// parameter can't be const
		void operator()( AEToken token ) const
		{
			::AEDisposeToken( &token );
		}
	};
	
	template <>
	struct LivelinessTraits< AEToken, TokenDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
	
	
	class AETokenEditor
	{
		private:
			Owned< AEToken, TokenDisposer >& token;
			AEToken workingCopy;
		
		public:
			AETokenEditor( Owned< AEToken, TokenDisposer >& token )
			:
				token      ( token           ),
				workingCopy( token.Release() )
			{}
			
			~AETokenEditor()
			{
				token = Owned< AEToken, TokenDisposer >::Seize( workingCopy );
			}
			
			AEToken& Get()       { return workingCopy; }
			operator AEToken&()  { return Get();       }
	};
	
	typedef AEDescList AETokenList;
	
	
	#pragma mark -
	#pragma mark ¥ Object Accessors ¥
	
	struct OSLAccessorUPP_Details: Basic_UPP_Details< ::OSLAccessorUPP,
	                                                  ::OSLAccessorProcPtr,
	                                                  ::NewOSLAccessorUPP,
	                                                  ::DisposeOSLAccessorUPP,
	                                                  ::InvokeOSLAccessorUPP >
	{};
	
	typedef UPP< OSLAccessorUPP_Details > OSLAccessorUPP;
	
	inline Owned< OSLAccessorUPP > NewOSLAccessorUPP( ::OSLAccessorProcPtr p )
	{
		return NewUPP< OSLAccessorUPP >( p );
	}
	
	inline void DisposeOSLAccessorUPP( Owned< OSLAccessorUPP > )  {}
	
	inline void InvokeOSLAccessorUPP( AEObjectClass   desiredClass,
	                                  const AEToken&  containerToken,
	                                  AEObjectClass   containerClass,
	                                  AEEnumerated    keyForm,
	                                  const AEDesc&   keyData,
	                                  AEToken&        value,
	                                  RefCon          accessorRefcon,
	                                  OSLAccessorUPP  userUPP )
	{
		ThrowOSStatus( userUPP( desiredClass, &containerToken, containerClass, keyForm, &keyData, &value, accessorRefcon ) );
	}
	
	struct OSLAccessor
	{
		AEObjectClass    desiredClass;
		DescType         containerType;
		OSLAccessorUPP   accessor;
		RefCon           accessorRefCon;
		bool             isSysHandler;
		
		OSLAccessor();
		
		OSLAccessor( AEObjectClass  desiredClass,
		             DescType       containerType,
		             OSLAccessorUPP accessor,
		             RefCon         accessorRefCon,
		             bool           isSysHandler )
		: 
			desiredClass  ( desiredClass   ),
			containerType ( containerType  ),
			accessor      ( accessor       ),
			accessorRefCon( accessorRefCon ),
			isSysHandler  ( isSysHandler   )
		{}
		
	};
	
	bool operator==( const OSLAccessor& a, const OSLAccessor& b );
	
	inline bool operator!=( const OSLAccessor& a, const OSLAccessor& b )
	{
		return !( a == b );
	}
	
	template <>
	struct Disposer< OSLAccessor > : public std::unary_function< OSLAccessor, void >,
	                                 private DefaultDestructionOSStatusPolicy
	{
		void operator()( const OSLAccessor& installation ) const
		{
			HandleDestructionOSStatus
			(
				::AERemoveObjectAccessor( installation.desiredClass,
				                          installation.containerType,
				                          installation.accessor,
				                          installation.isSysHandler )
			);
		}
	};
	
	typedef Owned< AEToken, TokenDisposer > ( *OSLAccessorProcPtr )( AEObjectClass   desiredClass,
	                                                                 const AEToken&  containerToken,
	                                                                 AEObjectClass   containerClass,
	                                                                 AEEnumerated    keyForm,
	                                                                 const AEDesc&   keyData,
	                                                                 RefCon          accessorRefcon );
	
	template < OSLAccessorProcPtr handler >
	struct Adapt_OSLAccessor
	{
		static pascal OSErr ToCallback( ::DescType      desiredClass,
		                                const AEToken*  containerToken,
		                                ::DescType      containerClass,
		                                ::DescType      keyForm,
		                                const AEDesc*   keyData,
		                                AEToken*         value,
		                                long            accessorRefcon )
		{
			try
			{
				*value = handler( desiredClass,
				                  *containerToken,
				                  containerClass,
				                  keyForm,
				                  *keyData,
				                  accessorRefcon ).Release();
			}
			catch ( OSStatus err )
			{
				return err;
			}
			return noErr;
		}
	};
	
	void AEObjectInit();
	
	Owned< AEToken, TokenDisposer > AEResolve( const AEObjectSpecifier&  objectSpecifier,
	                                           AEResolveCallbackFlags    callbackFlags );
	
	Owned< OSLAccessor > AEInstallObjectAccessor( const OSLAccessor& toInstall );
	
	inline Owned< OSLAccessor > AEInstallObjectAccessor( AEObjectClass    desiredClass,
	                                                     DescType         containerType,
	                                                     OSLAccessorUPP   accessor,
	                                                     RefCon           accessorRefCon = RefCon(),
	                                                     bool             isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             accessor,
		                                             accessorRefCon,
		                                             isSysHandler ) );
	}
	
	template < typename OSLAccessorUPP::ProcPtr accessor >
	Owned< OSLAccessor > AEInstallObjectAccessor( AEObjectClass  desiredClass,
	                                              DescType       containerType,
	                                              RefCon         accessorRefCon = RefCon(),
	                                              bool           isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             StaticUPP< OSLAccessorUPP, accessor >(),
		                                             accessorRefCon,
		                                             isSysHandler ) );
	}
	
	template < OSLAccessorProcPtr accessor >
	Owned< OSLAccessor > AEInstallObjectAccessor( AEObjectClass    desiredClass,
	                                              DescType         containerType,
	                                              RefCon           accessorRefCon = RefCon(),
	                                              bool             isSysHandler   = false )
	{
		return AEInstallObjectAccessor< Adapt_OSLAccessor< accessor >::ToCallback >
		(
			desiredClass, 
			containerType, 
			accessorRefCon, 
			isSysHandler
		);
	}
	
	inline void AERemoveObjectAccessor( Owned< OSLAccessor > )  {}
	
	typedef OSLAccessor AEGetObjectAccessor_Result;
	OSLAccessor AEGetObjectAccessor( AEObjectClass    desiredClass,
	                                 DescType         containerType,
	                                 bool             isSysHandler );
	
	#pragma mark -
	#pragma mark ¥ AEToken routines ¥
	
	template < ::DescType type >
	Owned< AEToken, TokenDisposer > AECreateToken( typename DescType_Traits< type >::Parameter data )
	{
		return Owned< AEToken, TokenDisposer >::Seize( AECreateDesc< type >( data ).Release() );
	}
	
	inline Owned< AEToken, TokenDisposer > AEDuplicateToken( const AEToken& token )
	{
		return Owned< AEToken, TokenDisposer >::Seize( AEDuplicateDesc( token ).Release() );
	}
	
	inline void AEPutPtr( Owned< AETokenList, TokenDisposer >&  tokenList,
	                      long                                  index,
	                      DescType                              typeCode,
	                      const void*                           dataPtr,
	                      Size                                  dataSize )
	{
		AEPutPtr( AETokenEditor( tokenList ), index, typeCode, dataPtr, dataSize );
	}
	
	inline void AEPutDesc( Owned< AETokenList, TokenDisposer >&  tokenList,
	                       long                                  index,
	                       const AEToken&                        token )
	{
		AEPutDesc( AETokenEditor( tokenList ), index, token );
	}
	
	inline Owned< AETokenList, TokenDisposer > AEGetNthToken( const AETokenList&  tokenList,
	                                                          long                index )
	{
		return Owned< AEToken, TokenDisposer >::Seize( AEGetNthDesc( tokenList, index ).Release() );
	}
	
	inline void AEPutKeyPtr( Owned< AERecord, TokenDisposer >&  record,
	                         AEKeyword                          keyword,
	                         DescType                           typeCode,
	                         const void*                        dataPtr,
	                         std::size_t                        dataSize )
	{
		AEPutKeyPtr( AETokenEditor( record ), keyword, typeCode, dataPtr, dataSize );
	}
	
	inline void AEPutKeyDesc( Owned< AERecord, TokenDisposer >&  record,
	                          AEKeyword                          keyword,
	                          const AEDesc&                      desc )
	{
		AEPutKeyDesc( AETokenEditor( record ), keyword, desc );
	}
	
	inline void AEPutKeyDesc( Owned< AERecord, TokenDisposer >&  record,
	                          const AEKeyDesc&                   keyDesc )
	{
		AEPutKeyDesc( AETokenEditor( record ), keyDesc );
	}
	
	inline void AEDisposeToken( Owned< AEToken, TokenDisposer > )  {}
	
}

#endif
