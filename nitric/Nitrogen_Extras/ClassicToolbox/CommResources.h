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

// Nitrogen
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif


namespace Nitrogen {
	
	void RegisterCommunicationResourceManagerErrors();
	
#if CALL_NOT_IN_CARBON
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	using ::CRMRec;
	using ::CRMRecPtr;
	
	struct CRMDeviceType_Tag  {};
	typedef IDType< CRMDeviceType_Tag, long, 0 > CRMDeviceType;
	
	struct CRMDeviceID_Tag  {};
	typedef IDType< CRMDeviceID_Tag, long, 0 > CRMDeviceID;
	
	struct CRMAttributes_Tag;
	typedef CRMAttributes_Tag* CRMAttributes;
	
	
	#pragma mark -
	#pragma mark ¥ CRM attributes disposal ¥
	
	template < long crmDeviceType >  struct CRMAttributes_Traits;
	
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
					Disposer< T >()( reinterpret_cast< T >( crmAttributes ) );
				}
			};
		
		public:
			CRMAttributesDisposer()  {}
			
			template < long crmDeviceType >
			void Register()
			{
				typedef typename CRMAttributes_Traits< crmDeviceType >::Type Type;
				
				map[ crmDeviceType ] = AdaptDisposer< Type >::ForCRMAttributes;
			}
			
			void Dispose( CRMDeviceType crmDeviceType, CRMAttributes crmAttributes );
	};
	
	CRMAttributesDisposer& TheGlobalCRMAttributesDisposer();
	
	template < long crmDeviceType >
	void RegisterCRMAttributesDisposer()
	{
		TheGlobalCRMAttributesDisposer().template Register< crmDeviceType >();
	}
	
	void DisposeCRMAttributes( CRMDeviceType  crmDeviceType,
	                           CRMAttributes  crmAttributes );
	
	#pragma mark -
	#pragma mark ¥ Specializations ¥
	
	template < long crmDeviceType >
	struct Disposer< CRMAttributes > : public std::unary_function< CRMAttributes, void >
	{
		void operator()( CRMAttributes crmAttributes ) const
		{
			DisposeCRMAttributes( crmDeviceType, crmAttributes );
		}
	};
	
	template <> struct Disposer< CRMRecPtr > : public std::unary_function< CRMRecPtr, void >
	{
		void operator()( CRMRecPtr crmRec ) const
		{
			DisposeCRMAttributes
			(
				crmRec->crmDeviceType,
				reinterpret_cast< CRMAttributes >( crmRec->crmAttributes )
			);
			
			Disposer< Ptr >()( crmRec );
		}
	};
	
	struct CRMRemover : public std::unary_function< CRMRecPtr, void >,
	                    private DefaultDestructionOSStatusPolicy
	{
		void operator()( CRMRecPtr crmRec ) const
		{
			OnlyOnce< RegisterCommunicationResourceManagerErrors >();
			HandleDestructionOSStatus( ::CRMRemove( crmRec ) );
		}
	};
	
	#pragma mark -
	#pragma mark ¥ CRM routines ¥
	
	void InitCRM();
	
	Owned< CRMRecPtr, CRMRemover > CRMInstall( Owned< CRMRecPtr > crmRec );
	
	Owned< CRMRecPtr > CRMRemove( Owned< CRMRecPtr, CRMRemover > crmRec );
	
	struct CRMSearch_Failed  {};
	
	CRMRecPtr CRMSearch( const CRMRec& crmRec );
	
	CRMRecPtr CRMSearch( CRMDeviceType  crmDeviceType,
	                     CRMDeviceID    crmDeviceID );
	
	template < long crmDeviceType >
	CRMRecPtr CRMSearch( CRMDeviceID crmDeviceID )
	{
		return CRMSearch( crmDeviceType, crmDeviceID );
	}
	
	Owned< CRMRecPtr > New_CRMRecord();
	
	template < long crmDeviceType >
	Owned< CRMRecPtr > New_CRMRecord( Owned< typename CRMAttributes_Traits< crmDeviceType >::Type > crmAttributes )
	{
		RegisterCRMAttributesDisposer< crmDeviceType >();
		
		Owned< CRMRecPtr > result = New_CRMRecord();
		
		CRMRecPtr crmRec = result;
		
		crmRec->crmDeviceType = crmDeviceType;
		crmRec->crmAttributes = reinterpret_cast< long >( crmAttributes.Release() );
		
		return result;
	}
	

#endif
	
}

#endif

