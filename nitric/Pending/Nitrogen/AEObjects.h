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
	#pragma mark ¥ AEToken ¥
	
	struct AETokenDisposer : public std::unary_function< AEToken, void >
	{
		// parameter can't be const
		void operator()( AEToken token ) const
		{
			::AEDisposeToken( &token );
		}
	};
	
	template <>
	struct LivelinessTraits< AEToken, AETokenDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
	
	
	class AETokenEditor
	{
		private:
			Owned< AEToken, AETokenDisposer >& token;
			AEToken workingCopy;
		
		public:
			AETokenEditor( Owned< AEToken, AETokenDisposer >& token )
			:
				token      ( token           ),
				workingCopy( token.Release() )
			{}
			
			~AETokenEditor()
			{
				token = Owned< AEToken, AETokenDisposer >::Seize( workingCopy );
			}
			
			AEToken& Get()       { return workingCopy; }
			operator AEToken&()  { return Get();       }
	};
	
	typedef AEDescList AETokenList;
	typedef DescType AECompOperator;
	
	
	#pragma mark -
	#pragma mark ¥ Callbacks ¥
	
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
	
	typedef Owned< AEToken, AETokenDisposer > ( *OSLAccessorProcPtr )( AEObjectClass   desiredClass,
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
	
#if 0
	
	struct OSLCompareUPP_Details: Basic_UPP_Details< ::OSLCompareUPP,
	                                                 ::OSLCompareProcPtr,
	                                                 ::NewOSLCompareUPP,
	                                                 ::DisposeOSLCompareUPP,
	                                                 ::InvokeOSLCompareUPP >
	{};
	
	typedef UPP< OSLCompareUPP_Details > OSLCompareUPP;
	
	inline Owned< OSLCompareUPP > NewOSLCompareUPP( ::OSLCompareProcPtr p )
	{
		return NewUPP< OSLCompareUPP >( p );
	}
	
	inline void DisposeOSLCompareUPP( Owned< OSLCompareUPP > )  {}
	
	inline void InvokeOSLCompareUPP( AECompOperator  oper,
	                                 const AEToken&  obj1,
	                                 const AEToken&  obj2,
	                                 ::Boolean&      result,
	                                 OSLCompareUPP   userUPP )
	{
		ThrowOSStatus( userUPP( oper, &obj1, &obj2, &result ) );
	}
	
	typedef bool ( *OSLCompareProcPtr )( AECompOperator  oper,
	                                     const AEToken&  obj1,
	                                     const AEToken&  obj2 );
	
#endif
	
	#pragma mark -
	#pragma mark ¥ Object Accessors ¥
	
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
	
	void AEObjectInit();
	
	void AESetObjectCallbacks( OSLCompareUPP        myCompareProc,
	                           OSLCountUPP          myCountProc,
	                           OSLDisposeTokenUPP   myDisposeTokenProc,
	                           OSLGetMarkTokenUPP   myGetMarkTokenProc,
	                           OSLMarkUPP           myMarkProc,
	                           OSLAdjustMarksUPP    myAdjustMarksProc,
	                           OSLGetErrDescUPP     myGetErrDescProcPtr );
	
#ifndef __APPLE_CC__
	
	void AESetObjectCallbacks( ::OSLCompareProcPtr       compareProc,
	                           ::OSLCountProcPtr         countProc,
	                           ::OSLDisposeTokenProcPtr  disposeTokenProc,
	                           ::OSLGetMarkTokenProcPtr  getMarkTokenProc,
	                           ::OSLMarkProcPtr          markProc,
	                           ::OSLAdjustMarksProcPtr   adjustMarksProc,
	                           ::OSLGetErrDescProcPtr    getErrDescProc );
	
#endif
	
	Owned< AEToken, AETokenDisposer > AEResolve( const AEObjectSpecifier&  objectSpecifier,
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
	
	Owned< AEToken, AETokenDisposer > AECallObjectAccessor( AEObjectClass   desiredClass,
	                                                        const AEToken&  containerToken,
	                                                        AEObjectClass   containerClass,
	                                                        AEEnumerated    keyForm,
	                                                        const AEDesc&   keyData );
	
	#pragma mark -
	#pragma mark ¥ AEToken routines ¥
	
	template < ::DescType type >
	Owned< AEToken, AETokenDisposer > AECreateToken( typename DescType_Traits< type >::Parameter data )
	{
		return Owned< AEToken, AETokenDisposer >::Seize( AECreateDesc< type >( data ).Release() );
	}
	
	inline Owned< AEToken, AETokenDisposer > AEDuplicateToken( const AEToken& token )
	{
		return Owned< AEToken, AETokenDisposer >::Seize( AEDuplicateDesc( token ).Release() );
	}
	
	inline void AEPutPtr( Owned< AETokenList, AETokenDisposer >&  tokenList,
	                      long                                    index,
	                      DescType                                typeCode,
	                      const void*                             dataPtr,
	                      Size                                    dataSize )
	{
		AEPutPtr( AETokenEditor( tokenList ), index, typeCode, dataPtr, dataSize );
	}
	
	inline void AEPutDesc( Owned< AETokenList, AETokenDisposer >&  tokenList,
	                       long                                    index,
	                       const AEToken&                          token )
	{
		AEPutDesc( AETokenEditor( tokenList ), index, token );
	}
	
	inline Owned< AETokenList, AETokenDisposer > AEGetNthToken( const AETokenList&  tokenList,
	                                                            long                index )
	{
		return Owned< AEToken, AETokenDisposer >::Seize( AEGetNthDesc( tokenList, index ).Release() );
	}
	
	inline void AEPutKeyPtr( Owned< AERecord, AETokenDisposer >&  record,
	                         AEKeyword                            keyword,
	                         DescType                             typeCode,
	                         const void*                          dataPtr,
	                         std::size_t                          dataSize )
	{
		AEPutKeyPtr( AETokenEditor( record ), keyword, typeCode, dataPtr, dataSize );
	}
	
	inline void AEPutKeyDesc( Owned< AERecord, AETokenDisposer >&  record,
	                          AEKeyword                            keyword,
	                          const AEDesc&                        desc )
	{
		AEPutKeyDesc( AETokenEditor( record ), keyword, desc );
	}
	
	inline void AEPutKeyDesc( Owned< AERecord, AETokenDisposer >&  record,
	                          const AEKeyDesc&                     keyDesc )
	{
		AEPutKeyDesc( AETokenEditor( record ), keyDesc );
	}
	
	inline void AEDisposeToken( Owned< AEToken, AETokenDisposer > )  {}
	
}

#endif
