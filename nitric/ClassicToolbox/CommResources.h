/*	===============
 *	CommResources.h
 *	===============
 */

#ifndef CLASSICTOOLBOX_COMMRESOURCES_H
#define CLASSICTOOLBOX_COMMRESOURCES_H

// Universal Interfaces
#ifndef __COMMRESOURCES__
#include <CommResources.h>
#endif

// Nucleus
#ifndef NUCLEUS_ADVANCEUNTILFAILURECONTAINER_H
#include "Nucleus/AdvanceUntilFailureContainer.h"
#endif
#include "Nucleus/ErrorsRegistered.h"
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif

// Nitrogen
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


enum
{
	typeCRMRecPtr = 'CRM '
};

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( CommunicationResourceManager );
	
	
#if CALL_NOT_IN_CARBON
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	using ::CRMRec;
	using ::CRMRecPtr;
	
	typedef Nucleus::Selector< struct CRMDeviceType_Tag, long >::Type CRMDeviceType;
	typedef Nucleus::ID      < struct CRMDeviceID_Tag,   long >::Type CRMDeviceID;
	
	typedef struct CRMAttributes_Tag* CRMAttributes;
	
	
	#pragma mark -
	#pragma mark ¥ CRM attributes ¥
	
	template < CRMDeviceType crmDeviceType >  struct CRMAttributes_Traits;
	
	template < CRMDeviceType crmDeviceType >
	typename CRMAttributes_Traits< crmDeviceType >::Type
	GetCRMAttributes( CRMRecPtr crmRec )
	{
		typedef typename CRMAttributes_Traits< crmDeviceType >::Type Type;
		return reinterpret_cast< Type >( crmRec->crmAttributes );
	}
	
	template < CRMDeviceType crmDeviceType >
	struct CRMAttributes_Getter : public std::unary_function< CRMRecPtr, typename CRMAttributes_Traits< crmDeviceType >::Type >
	{
		typedef typename CRMAttributes_Traits< crmDeviceType >::Type Type;
		
		Type operator()( CRMRecPtr crmRec ) const
		{
			return GetCRMAttributes< crmDeviceType >( crmRec );
		}
	};
	
	class CRMAttributesDisposer
	{
		public:
			typedef void (*DisposerType)( CRMAttributes );
		
		private:
			typedef std::map< CRMDeviceType, DisposerType >  Map;
			
			Map map;
			
			// not implemented:
			CRMAttributesDisposer( const CRMAttributesDisposer& );
			CRMAttributesDisposer& operator=( const CRMAttributesDisposer& );
			
			template < class T >
			struct AdaptDisposer
			{
				static void ForCRMAttributes( CRMAttributes crmAttributes )
				{
					Nucleus::Disposer< T >()( reinterpret_cast< T >( crmAttributes ) );
				}
			};
		
		public:
			CRMAttributesDisposer()  {}
			
			template < CRMDeviceType crmDeviceType >
			void Register()
			{
				typedef typename CRMAttributes_Traits< crmDeviceType >::Type Type;
				
				map[ crmDeviceType ] = AdaptDisposer< Type >::ForCRMAttributes;
			}
			
			void Dispose( CRMDeviceType crmDeviceType, CRMAttributes crmAttributes );
	};
	
	CRMAttributesDisposer& TheGlobalCRMAttributesDisposer();
	
	template < CRMDeviceType crmDeviceType >
	void RegisterCRMAttributesDisposer()
	{
		TheGlobalCRMAttributesDisposer().template Register< crmDeviceType >();
	}
	
	void DisposeCRMAttributes( CRMDeviceType  crmDeviceType,
	                           CRMAttributes  crmAttributes );
	
}

namespace Nucleus
{
	
	#pragma mark -
	#pragma mark ¥ Specializations ¥
	
	template < Nitrogen::CRMDeviceType crmDeviceType >
	struct Disposer< Nitrogen::CRMAttributes > : public std::unary_function< Nitrogen::CRMAttributes, void >
	{
		void operator()( Nitrogen::CRMAttributes crmAttributes ) const
		{
			Nitrogen::DisposeCRMAttributes( crmDeviceType, crmAttributes );
		}
	};
	
	template <> struct Disposer< CRMRecPtr > : public std::unary_function< CRMRecPtr, void >
	{
		void operator()( CRMRecPtr crmRec ) const
		{
			Nitrogen::DisposeCRMAttributes
			(
				Nitrogen::CRMDeviceType( crmRec->crmDeviceType ),
				reinterpret_cast< Nitrogen::CRMAttributes >( crmRec->crmAttributes )
			);
			
			Disposer< Nitrogen::Ptr >()( crmRec );
		}
	};
	
}

namespace Nitrogen
{
	
	struct CRMRemover : public std::unary_function< CRMRecPtr, void >,
	                    private DefaultDestructionOSStatusPolicy
	{
		void operator()( CRMRecPtr crmRec ) const
		{
			HandleDestructionOSStatus( ::CRMRemove( crmRec ) );
		}
	};
	
	#pragma mark -
	#pragma mark ¥ CRM routines ¥
	
	void InitCRM();
	
	Nucleus::Owned< CRMRecPtr, CRMRemover > CRMInstall( Nucleus::Owned< CRMRecPtr > crmRec );
	
	Nucleus::Owned< CRMRecPtr > CRMRemove( Nucleus::Owned< CRMRecPtr, CRMRemover > crmRec );
	
	struct CRMSearch_Failed  {};
	
	CRMRecPtr CRMSearch( const CRMRec& crmRec );
	
	CRMRecPtr CRMSearch( CRMDeviceType  crmDeviceType,
	                     CRMDeviceID    crmDeviceID );
	
	template < CRMDeviceType crmDeviceType >
	CRMRecPtr CRMSearch( CRMDeviceID crmDeviceID )
	{
		return CRMSearch( crmDeviceType, crmDeviceID );
	}
	
	inline bool CRMIsDriverOpen( ConstStr255Param driverName )
	{
		return ::CRMIsDriverOpen( driverName );
	}
	
	Nucleus::Owned< CRMRecPtr > New_CRMRecord();
	
	template < CRMDeviceType crmDeviceType >
	Nucleus::Owned< CRMRecPtr > New_CRMRecord( Nucleus::Owned< typename CRMAttributes_Traits< crmDeviceType >::Type > crmAttributes )
	{
		RegisterCRMAttributesDisposer< crmDeviceType >();
		
		Nucleus::Owned< CRMRecPtr > result = New_CRMRecord();
		
		CRMRecPtr crmRec = result;
		
		crmRec->crmDeviceType = crmDeviceType;
		crmRec->crmAttributes = reinterpret_cast< long >( crmAttributes.Release() );
		
		return result;
	}
	
	#pragma mark -
	#pragma mark ¥ Paraphernalia ¥
	
	class CRMResource_Container_Specifics
	{
		private:
			CRMDeviceType crmDeviceType;
		
		public:
			typedef CRMRecPtr value_type;
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			typedef value_type key_type;
			
			typedef CRMSearch_Failed EndOfEnumeration;
			
			CRMResource_Container_Specifics( CRMDeviceType crmDeviceType )
			:
				crmDeviceType( crmDeviceType )
			{}
			
			key_type GetNextKey( const key_type& value ) const
			{
				return CRMSearch( crmDeviceType,
				                  CRMDeviceID( value != NULL ? value->crmDeviceID : 0 ) );
			}
			
			static const key_type* GetPointer( const key_type& value )  { return &value; }
			
			       key_type begin_key() const  { return GetNextKey( NULL ); }
			static key_type end_key  ()        { return             NULL  ; }
	};
	
	class CRMResource_Container: public Nucleus::AdvanceUntilFailureContainer< CRMResource_Container_Specifics >
	{
		friend CRMResource_Container CRMResources( CRMDeviceType crmDeviceType );
		
		private:
			CRMResource_Container( CRMDeviceType crmDeviceType )
			:
				Nucleus::AdvanceUntilFailureContainer< CRMResource_Container_Specifics >
				(
					CRMResource_Container_Specifics( crmDeviceType )
				)
			{}
	};
	
	inline CRMResource_Container CRMResources( CRMDeviceType crmDeviceType )
	{
		return CRMResource_Container( crmDeviceType );
	}
	

#endif
	
}

#endif

