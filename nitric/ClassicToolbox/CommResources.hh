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
#ifndef NUCLEUS_ADVANCEUNTILDONESEQUENCE_HH
#include "nucleus/advance_until_done_sequence.hh"
#endif
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif

// Nitrogen
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif


enum
{
	typeCRMRecPtr = 'CRM '
};

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( CommunicationResourceManager );
	
	
	#pragma mark -
	#pragma mark ** Types **
	
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
	#pragma mark ** CRM attributes **
	
	template < CRMDeviceType crmDeviceType >  struct CRMAttributes_Traits;
	
	template < CRMDeviceType crmDeviceType >
	typename CRMAttributes_Traits< crmDeviceType >::Type
	GetCRMAttributes( CRMRecPtr crmRec )
	{
		typedef typename CRMAttributes_Traits< crmDeviceType >::Type Type;
		return reinterpret_cast< Type >( crmRec->crmAttributes );
	}
	
	template < CRMDeviceType crmDeviceType >
	struct CRMAttributes_Getter
	{
		typedef CRMRecPtr                                             argument_type;
		typedef typename CRMAttributes_Traits< crmDeviceType >::Type  result_type;
		
		result_type operator()( CRMRecPtr crmRec ) const
		{
			return GetCRMAttributes< crmDeviceType >( crmRec );
		}
	};
	
	typedef void (*CRMAttributes_Disposer)( CRMAttributes );
	
	void DisposeCRMAttributes( CRMDeviceType  crmDeviceType,
	                           CRMAttributes  crmAttributes );
	
}

namespace nucleus
{
	
	#pragma mark -
	#pragma mark ** Specializations **
	
	template < Nitrogen::CRMDeviceType crmDeviceType >
	struct disposer< Nitrogen::CRMAttributes >
	{
		typedef Nitrogen::CRMAttributes  argument_type;
		typedef void                     result_type;
		
		void operator()( Nitrogen::CRMAttributes crmAttributes ) const
		{
			Nitrogen::DisposeCRMAttributes( crmDeviceType, crmAttributes );
		}
	};
	
	template <> struct disposer< CRMRecPtr >
	{
		typedef CRMRecPtr  argument_type;
		typedef void       result_type;
		
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
	
	struct CRMRemover
	{
		typedef CRMRecPtr  argument_type;
		typedef void       result_type;
		
		void operator()( CRMRecPtr crmRec ) const
		{
			(void) ::CRMRemove( crmRec );
		}
	};
	
	#pragma mark -
	#pragma mark ** CRM routines **
	
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
	
	#pragma mark -
	#pragma mark ** Paraphernalia **
	
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
			
			key_type get_next_key( const key_type& value ) const
			{
				return CRMSearch( crmDeviceType,
				                  CRMDeviceID( value != NULL ? value->crmDeviceID : 0 ) );
			}
			
			static const key_type* GetPointer( const key_type& value )  { return &value; }
			
			       key_type begin_key() const  { return get_next_key( NULL ); }
			static key_type end_key  ()        { return               NULL  ; }
	};
	
	class CRMResource_Container : public nucleus::advance_until_done_sequence< CRMResource_Container_Specifics >
	{
		friend CRMResource_Container CRMResources( CRMDeviceType crmDeviceType );
		
		private:
			CRMResource_Container( CRMDeviceType crmDeviceType )
			:
				nucleus::advance_until_done_sequence< CRMResource_Container_Specifics >
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
