/*	================
 *	CommResources.cp
 *	================
 */

#include "ClassicToolbox/CommResources.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( CommunicationResourceManager )
	
	
	static void RegisterCommunicationResourceManagerErrors();
	
	
#pragma force_active on
	
	class CommunicationResourceManagerErrorsRegistration
	{
		public:
			CommunicationResourceManagerErrorsRegistration()  { RegisterCommunicationResourceManagerErrors(); }
	};
	
	static CommunicationResourceManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
	void CRMAttributesDisposer::Dispose( CRMDeviceType crmDeviceType, CRMAttributes crmAttributes )
	{
		Map::const_iterator found = map.find( crmDeviceType );
		if ( found != map.end() )
		{
			return found->second( crmAttributes );
		}
		// FIXME:  Assert
	}
	
	CRMAttributesDisposer& TheGlobalCRMAttributesDisposer()
	{
		static CRMAttributesDisposer theGlobalCRMAttributesDisposer;
		return theGlobalCRMAttributesDisposer;
	}
	
	void DisposeCRMAttributes( CRMDeviceType crmDeviceType, CRMAttributes crmAttributes )
	{
		TheGlobalCRMAttributesDisposer().Dispose( crmDeviceType, crmAttributes );
	}
	
	void InitCRM()
	{
		ThrowOSStatus( ::InitCRM() );
	}
	
	Nucleus::Owned< CRMRecPtr, CRMRemover > CRMInstall( Nucleus::Owned< CRMRecPtr > crmRec )
	{
		// ::CRMInstall() returns void.  There's no means for reporting errors.
		::CRMInstall( crmRec.Get() );
		return Nucleus::Owned< CRMRecPtr, CRMRemover >::Seize( crmRec.Release() );
	}
	
	Nucleus::Owned< CRMRecPtr > CRMRemove( Nucleus::Owned< CRMRecPtr, CRMRemover > crmRec )
	{
		ThrowOSStatus( ::CRMRemove( crmRec.Get() ) );
		
		return Nucleus::Owned< CRMRecPtr >::Seize( crmRec.Release() );
	}
	
	CRMRecPtr CRMSearch( const CRMRec& crmRec )
	{
		CRMRecPtr result = ::CRMSearch( const_cast< CRMRecPtr >( &crmRec ) );
		
		return result;
	}
	
	CRMRecPtr CRMSearch( CRMDeviceType crmDeviceType, CRMDeviceID crmDeviceID )
	{
		CRMRec crmRec;
		crmRec.crmDeviceType = crmDeviceType;
		crmRec.crmDeviceID   = crmDeviceID;
		
		return CRMSearch( crmRec );
	}
	
	Nucleus::Owned< CRMRecPtr > New_CRMRecord()
	{
		Nucleus::Owned< CRMRecPtr > result = Nucleus::Owned< CRMRecPtr >::Seize
		(
			reinterpret_cast< CRMRecPtr >
			(
				NewPtrSysClear( sizeof (CRMRec) ).Release().Get()
			)
		);
		
		CRMRecPtr crmRec = result;
		
		crmRec->qType = crmType;
		crmRec->crmDeviceID = 0;
		
		return result;
	}
	
	void RegisterCommunicationResourceManagerErrors()
	{
		// FIXME
	}
	
	namespace Tests
	{
		
		static void Test1( CRMDeviceType crmDeviceType )
		{
			CRMResource_Container crmResources = CRMResources( crmDeviceType );
		}
	}
	
}

