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

namespace Nitrogen {
	
	class AELogicalOperator_Tag {};
	typedef SelectorType< AELogicalOperator_Tag, ::FourCharCode > AELogicalOperator;
	typedef AELogicalOperator AELogicalDescriptor;
	
	class AEAbsoluteOrdinal_Tag {};
	typedef SelectorType< AEAbsoluteOrdinal_Tag, ::FourCharCode > AEAbsoluteOrdinal;
	
	class AERelativeOrdinal_Tag {};
	typedef SelectorType< AERelativeOrdinal_Tag, ::FourCharCode > AERelativeOrdinal;
	typedef AERelativeOrdinal AERelativeDescriptor;
	
	struct AEResolveCallbackFlags_Tag  {};
	typedef FlagType< AEResolveCallbackFlags_Tag, short, ::kAEIDoMinimum > AEResolveCallbackFlags;
	
	
	// *** LOGICAL OPERATOR CONSTANTS  ***
	inline AELogicalOperator AEAND     ()  { return AELogicalOperator::Make( kAEAND      ); }
	inline AELogicalOperator AEOR      ()  { return AELogicalOperator::Make( kAEOR       ); }
	inline AELogicalOperator AENOT     ()  { return AELogicalOperator::Make( kAENOT      ); }
	
	// *** ABSOLUTE ORDINAL CONSTANTS  ***
	inline AEAbsoluteOrdinal AEFirst   ()  { return AEAbsoluteOrdinal::Make( kAEFirst    ); }
	inline AEAbsoluteOrdinal AELast    ()  { return AEAbsoluteOrdinal::Make( kAELast     ); }
	inline AEAbsoluteOrdinal AEMiddle  ()  { return AEAbsoluteOrdinal::Make( kAEMiddle   ); }
	inline AEAbsoluteOrdinal AEAny     ()  { return AEAbsoluteOrdinal::Make( kAEAny      ); }
	inline AEAbsoluteOrdinal AEAll     ()  { return AEAbsoluteOrdinal::Make( kAEAll      ); }
	
	// *** RELATIVE ORDINAL CONSTANTS  ***
	inline AERelativeOrdinal AENext    ()  { return AERelativeOrdinal::Make( kAENext     ); }
	inline AERelativeOrdinal AEPrevious()  { return AERelativeOrdinal::Make( kAEPrevious ); }
	
	
	// *** KEYWORD CONSTANT    ***
	inline AEKeyword KeyAECompOperator   ()  { return AEKeyword::Make( keyAECompOperator    ); }
	inline AEKeyword KeyAELogicalTerms   ()  { return AEKeyword::Make( keyAELogicalTerms    ); }
	inline AEKeyword KeyAELogicalOperator()  { return AEKeyword::Make( keyAELogicalOperator ); }
	inline AEKeyword KeyAEObject1        ()  { return AEKeyword::Make( keyAEObject1         ); }
	inline AEKeyword KeyAEObject2        ()  { return AEKeyword::Make( keyAEObject2         ); }
	
	//     ... for Keywords for getting fields out of object specifier records. 
	inline AEKeyword KeyAEDesiredClass   ()  { return AEKeyword::Make( keyAEDesiredClass    ); }
	inline AEKeyword KeyAEContainer      ()  { return AEKeyword::Make( keyAEContainer       ); }
	inline AEKeyword KeyAEKeyForm        ()  { return AEKeyword::Make( keyAEKeyForm         ); }
	inline AEKeyword KeyAEKeyData        ()  { return AEKeyword::Make( keyAEKeyData         ); }

	//     ... for Keywords for getting fields out of Range specifier records. 
	inline AEKeyword KeyAERangeStart     ()  { return AEKeyword::Make( keyAERangeStart      ); }
	inline AEKeyword KeyAERangeStop      ()  { return AEKeyword::Make( keyAERangeStop       ); }
	
	//     ... special handler selectors for OSL Callbacks. 
	inline AEKeyword KeyDisposeTokenProc ()  { return AEKeyword::Make( keyDisposeTokenProc  ); }
	inline AEKeyword KeyAECompareProc    ()  { return AEKeyword::Make( keyAECompareProc     ); }
	inline AEKeyword KeyAECountProc      ()  { return AEKeyword::Make( keyAECountProc       ); }
	inline AEKeyword KeyAEMarkTokenProc  ()  { return AEKeyword::Make( keyAEMarkTokenProc   ); }
	inline AEKeyword KeyAEMarkProc       ()  { return AEKeyword::Make( keyAEMarkProc        ); }
	inline AEKeyword KeyAEAdjustMarksProc()  { return AEKeyword::Make( keyAEAdjustMarksProc ); }
	inline AEKeyword KeyAEGetErrDescProc ()  { return AEKeyword::Make( keyAEGetErrDescProc  ); }

// ***   VALUE and TYPE CONSTANTS    ***
	
	//     ... possible values for the keyAEKeyForm field of an object specifier.
	inline AEEnumerated FormAbsolutePosition()  { return AEEnumerated::Make( formAbsolutePosition ); }
	inline AEEnumerated FormRelativePosition()  { return AEEnumerated::Make( formRelativePosition ); }
	inline AEEnumerated FormTest            ()  { return AEEnumerated::Make( formTest             ); }
	inline AEEnumerated FormRange           ()  { return AEEnumerated::Make( formRange            ); }
	inline AEEnumerated FormPropertyID      ()  { return AEEnumerated::Make( formPropertyID       ); }
	inline AEEnumerated FormName            ()  { return AEEnumerated::Make( formName             ); }
	
	//     ... relevant types (some of these are often pared with forms above).
	inline DescType TypeObjectSpecifier    ()  { return DescType::Make( typeObjectSpecifier     ); }
	inline DescType TypeObjectBeingExamined()  { return DescType::Make( typeObjectBeingExamined ); }
	inline DescType TypeCurrentContainer   ()  { return DescType::Make( typeCurrentContainer    ); }
	inline DescType TypeToken              ()  { return DescType::Make( typeToken               ); }
	inline DescType TypeRelativeDescriptor ()  { return DescType::Make( typeRelativeDescriptor  ); }
	inline DescType TypeAbsoluteOrdinal    ()  { return DescType::Make( typeAbsoluteOrdinal     ); }
	inline DescType TypeIndexDescriptor    ()  { return DescType::Make( typeIndexDescriptor     ); }
	inline DescType TypeRangeDescriptor    ()  { return DescType::Make( typeRangeDescriptor     ); }
	inline DescType TypeLogicalDescriptor  ()  { return DescType::Make( typeLogicalDescriptor   ); }
	inline DescType TypeCompDescriptor     ()  { return DescType::Make( typeCompDescriptor      ); }
	inline DescType TypeOSLTokenList       ()  { return DescType::Make( typeOSLTokenList        ); }

//  Possible values for flags parameter to AEResolve.  They're additive
	inline AEResolveCallbackFlags AEIDoMinimum          ()  { return AEResolveCallbackFlags::Make( kAEIDoMinimum           ); }
	inline AEResolveCallbackFlags AEIDoWhose            ()  { return AEResolveCallbackFlags::Make( kAEIDoWhose             ); }
	inline AEResolveCallbackFlags AEIDoMarking          ()  { return AEResolveCallbackFlags::Make( kAEIDoMarking           ); }
	inline AEResolveCallbackFlags AEPassSubDescs        ()  { return AEResolveCallbackFlags::Make( kAEPassSubDescs         ); }
	inline AEResolveCallbackFlags AEResolveNestedLists  ()  { return AEResolveCallbackFlags::Make( kAEResolveNestedLists   ); }
	inline AEResolveCallbackFlags AEHandleSimpleRanges  ()  { return AEResolveCallbackFlags::Make( kAEHandleSimpleRanges   ); }
	inline AEResolveCallbackFlags AEUseRelativeIterators()  { return AEResolveCallbackFlags::Make( kAEUseRelativeIterators ); }

// *** SPECIAL CONSTANTS FOR CUSTOM WHOSE-CLAUSE RESOLUTION
	
	inline DescType TypeWhoseDescriptor    ()  { return DescType::Make( typeWhoseDescriptor     ); }
	
	inline AEEnumerated FormWhose           ()  { return AEEnumerated::Make( formWhose            ); }
	
	inline DescType TypeWhoseRange         ()  { return DescType::Make( typeWhoseRange          ); }
	
	inline AEKeyword KeyAEWhoseRangeStart()  { return AEKeyword::Make( keyAEWhoseRangeStart ); }
	inline AEKeyword KeyAEWhoseRangeStop ()  { return AEKeyword::Make( keyAEWhoseRangeStop  ); }
	inline AEKeyword KeyAEIndex          ()  { return AEKeyword::Make( keyAEIndex           ); }
	inline AEKeyword KeyAETest           ()  { return AEKeyword::Make( keyAETest            ); }
	
	
	struct TokenDisposer : public std::unary_function< AEDesc, void >
	{
		// parameter can't be const
		void operator()( AEDesc token ) const
		{
			::AEDisposeToken( &token );
		}
	};
	
	template <>
	struct LivelinessTraits< AEDesc, TokenDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
	
	
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
	
	inline void InvokeOSLAccessorUPP( DescType desiredClass,
	                                  const AEDesc& container,
	                                  DescType containerClass,
	                                  DescType form,
	                                  const AEDesc& selectionData,
	                                  AEDesc& value, 
	                                  RefCon accessorRefcon,
	                                  OSLAccessorUPP userUPP )
	{
		ThrowOSStatus( userUPP( desiredClass, &container, containerClass, form, &selectionData, &value, accessorRefcon ) );
	}
	
	struct OSLAccessor
	{
		DescType         desiredClass;
		DescType         containerType;
		OSLAccessorUPP   accessor;
		RefCon           accessorRefCon;
		bool             isSysHandler;
		
		OSLAccessor();
		
		OSLAccessor( DescType       desiredClass,
		             DescType       containerType,
		             OSLAccessorUPP accessor,
		             RefCon         accessorRefCon,
		             bool           isSysHandler )
		: 
			desiredClass( desiredClass ),
			containerType( containerType ),
			accessor( accessor ),
			accessorRefCon( accessorRefCon ),
			isSysHandler( isSysHandler )
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
			DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus
			(
				::AERemoveObjectAccessor( installation.desiredClass,
				                          installation.containerType,
				                          installation.accessor,
				                          installation.isSysHandler )
			);
		}
	};
	
	typedef Owned< AEDesc, TokenDisposer > ( *OSLAccessorProcPtr )( DescType desiredClass, const AEDesc& container, DescType containerClass, DescType form, const AEDesc& selectionData, RefCon accessorRefcon );
	
	template < OSLAccessorProcPtr handler >
	struct Adapt_OSLAccessor
	{
		static pascal OSErr ToCallback( ::DescType desiredClass, const AEDesc* container, ::DescType containerClass, ::DescType form, const AEDesc* selectionData, AEDesc* value, long accessorRefcon )
		{
			try
			{
				*result = handler( desiredClass, *container, containerClass, form, *selectionData, accessorRefcon ).Release();
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			return noErr;
		}
	};
	
	void AEObjectInit();
	
	Owned< AEDesc, TokenDisposer >
	AEResolve( const AEDesc& objectSpecifier, AEResolveCallbackFlags callbackFlags );
	
	Owned< OSLAccessor > AEInstallObjectAccessor( const OSLAccessor& toInstall );
	
	inline Owned< OSLAccessor > AEInstallObjectAccessor
	(
		DescType         desiredClass,
		DescType         containerType,
		OSLAccessorUPP   accessor,
		RefCon           accessorRefCon = RefCon(),
		bool             isSysHandler = false
	)
	{
		return AEInstallObjectAccessor
		(
			OSLAccessor( desiredClass, containerType, accessor, accessorRefCon, isSysHandler )
		);
	}
	
	template < typename OSLAccessorUPP::ProcPtr accessor >
	Owned< OSLAccessor > AEInstallObjectAccessor
	(
		DescType         desiredClass,
		DescType         containerType,
		RefCon           accessorRefCon = RefCon(),
		bool             isSysHandler = false
	)
	{
		return AEInstallObjectAccessor
		(
			OSLAccessor
			(
				desiredClass, 
				containerType, 
				StaticUPP< OSLAccessorUPP, accessor >(), 
				accessorRefCon, 
				isSysHandler
			)
		);
	}
	
	template < AECoerceDescProcPtr accessor >
	Owned< OSLAccessor > AEInstallObjectAccessor
	(
		DescType         desiredClass,
		DescType         containerType,
		RefCon           accessorRefCon = RefCon(),
		bool             isSysHandler = false
	)
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
	OSLAccessor AEGetObjectAccessor
	(
		DescType         desiredClass,
		DescType         containerType,
		bool             isSysHandler
	);
	
	inline void AEDisposeToken( Owned< AEDesc, TokenDisposer > )  {}
	
}

#endif
