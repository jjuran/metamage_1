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
	
	struct AEResolveCallbackFlags_Tag  {};
	typedef FlagType< AEResolveCallbackFlags_Tag, short, kAEIDoMinimum > AEResolveCallbackFlags;
	
	
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
