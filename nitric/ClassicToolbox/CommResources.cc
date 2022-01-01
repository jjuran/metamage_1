/*	================
 *	CommResources.cc
 *	================
 */

#include "ClassicToolbox/CommResources.hh"

// Standard C++
#include <map>

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( CommunicationResourceManager )
	
	
	static void RegisterCommunicationResourceManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class CommunicationResourceManagerErrorsRegistration
	{
		public:
			CommunicationResourceManagerErrorsRegistration()  { RegisterCommunicationResourceManagerErrors(); }
	};
	
	static CommunicationResourceManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	typedef std::map< CRMDeviceType, CRMAttributes_Disposer >  CRMAttributes_Disposer_Map;
	
	CRMAttributes_Disposer_Map gCRMAttributes_Disposer_Map;
	
	
	void DisposeCRMAttributes( CRMDeviceType crmDeviceType, CRMAttributes crmAttributes )
	{
		CRMAttributes_Disposer_Map::const_iterator it = gCRMAttributes_Disposer_Map.find( crmDeviceType );
		
		if ( it != gCRMAttributes_Disposer_Map.end() )
		{
			return it->second( crmAttributes );
		}
		
		// FIXME:  Assert
	}
	
	void InitCRM()
	{
		Mac::ThrowOSStatus( ::InitCRM() );
	}
	
	nucleus::owned< CRMRecPtr, CRMRemover > CRMInstall( nucleus::owned< CRMRecPtr > crmRec )
	{
		// ::CRMInstall() returns void.  There's no means for reporting errors.
		::CRMInstall( crmRec.get() );
		return nucleus::owned< CRMRecPtr, CRMRemover >::seize( crmRec.release() );
	}
	
	nucleus::owned< CRMRecPtr > CRMRemove( nucleus::owned< CRMRecPtr, CRMRemover > crmRec )
	{
		Mac::ThrowOSStatus( ::CRMRemove( crmRec.get() ) );
		
		return nucleus::owned< CRMRecPtr >::seize( crmRec.release() );
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
	
	nucleus::owned< CRMRecPtr > New_CRMRecord()
	{
		nucleus::owned< CRMRecPtr > result = nucleus::owned< CRMRecPtr >::seize
		(
			reinterpret_cast< CRMRecPtr >
			(
				NewPtrSysClear( sizeof (CRMRec) ).release().Get()
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
