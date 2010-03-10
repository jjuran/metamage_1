/*	================
 *	CommResources.hh
 *	================
 */

#ifndef CLASSICTOOLBOX_COMMRESOURCES_HH
#define CLASSICTOOLBOX_COMMRESOURCES_HH

// Mac OS
#ifndef __COMMRESOURCES__
#include <CommResources.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"

// Nucleus
#include "Nucleus/AdvanceUntilDoneSequence.h"
#include "Nucleus/ErrorsRegistered.h"

// Nitrogen
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif


enum
{
	typeCRMRecPtr = 'CRM '
};

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( CommunicationResourceManager );
	
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	using ::CRMRec;
	using ::CRMRecPtr;
	
	enum CRMDeviceType
	{
		kCRMDeviceType_Max = nucleus::enumeration_traits< long >::max
	};
	
	enum CRMDeviceID
	{
		kCRMDeviceID_Max = nucleus::enumeration_traits< long >::max
	};
	
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
					nucleus::disposer< T >()( reinterpret_cast< T >( crmAttributes ) );
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

namespace nucleus
{
	
	#pragma mark -
	#pragma mark ¥ Specializations ¥
	
	template < Nitrogen::CRMDeviceType crmDeviceType >
	struct disposer< Nitrogen::CRMAttributes > : public std::unary_function< Nitrogen::CRMAttributes, void >
	{
		void operator()( Nitrogen::CRMAttributes crmAttributes ) const
		{
			Nitrogen::DisposeCRMAttributes( crmDeviceType, crmAttributes );
		}
	};
	
	template <> struct disposer< CRMRecPtr > : public std::unary_function< CRMRecPtr, void >
	{
		void operator()( CRMRecPtr crmRec ) const
		{
			Nitrogen::DisposeCRMAttributes
			(
				Nitrogen::CRMDeviceType( crmRec->crmDeviceType ),
				reinterpret_cast< Nitrogen::CRMAttributes >( crmRec->crmAttributes )
			);
			
			disposer< Nitrogen::Ptr >()( crmRec );
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
	
	nucleus::owned< CRMRecPtr, CRMRemover > CRMInstall( nucleus::owned< CRMRecPtr > crmRec );
	
	nucleus::owned< CRMRecPtr > CRMRemove( nucleus::owned< CRMRecPtr, CRMRemover > crmRec );
	
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
	
	nucleus::owned< CRMRecPtr > New_CRMRecord();
	
	template < CRMDeviceType crmDeviceType >
	nucleus::owned< CRMRecPtr > New_CRMRecord( nucleus::owned< typename CRMAttributes_Traits< crmDeviceType >::Type > crmAttributes )
	{
		RegisterCRMAttributesDisposer< crmDeviceType >();
		
		nucleus::owned< CRMRecPtr > result = New_CRMRecord();
		
		CRMRecPtr crmRec = result;
		
		crmRec->crmDeviceType = crmDeviceType;
		crmRec->crmAttributes = reinterpret_cast< long >( crmAttributes.release() );
		
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
	
	class CRMResource_Container: public Nucleus::AdvanceUntilDoneSequence< CRMResource_Container_Specifics >
	{
		friend CRMResource_Container CRMResources( CRMDeviceType crmDeviceType );
		
		private:
			CRMResource_Container( CRMDeviceType crmDeviceType )
			:
				Nucleus::AdvanceUntilDoneSequence< CRMResource_Container_Specifics >
				(
					CRMResource_Container_Specifics( crmDeviceType )
				)
			{}
	};
	
	inline CRMResource_Container CRMResources( CRMDeviceType crmDeviceType )
	{
		return CRMResource_Container( crmDeviceType );
	}
	
}

#endif

