/*	================
 *	CommResources.cp
 *	================
 */

#ifndef CLASSICTOOLBOX_COMMRESOURCES_H
#include "ClassicToolbox/CommResources.h"
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
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
		OnlyOnce< RegisterCommunicationResourceManagerErrors >();
		
		ThrowOSStatus( ::InitCRM() );
	}
	
	Owned< CRMRecPtr, CRMRemover > CRMInstall( Owned< CRMRecPtr > crmRec )
	{
		// ::CRMInstall() returns void.  There's no means for reporting errors.
		::CRMInstall( crmRec.Get() );
		return Owned< CRMRecPtr, CRMRemover >::Seize( crmRec.Release() );
	}
	
	Owned< CRMRecPtr > CRMRemove( Owned< CRMRecPtr, CRMRemover > crmRec )
	{
		OnlyOnce< RegisterCommunicationResourceManagerErrors >();
		
		ThrowOSStatus( ::CRMRemove( crmRec.Get() ) );
		return Owned< CRMRecPtr >::Seize( crmRec.Release() );
	}
	
	CRMRecPtr CRMSearch( const CRMRec& crmRec )
	{
		CRMRecPtr result = ::CRMSearch( const_cast< CRMRecPtr >( &crmRec ) );
		
		if ( result == NULL )
		{
			throw CRMSearch_Failed();
		}
		
		return result;
	}
	
	CRMRecPtr CRMSearch( CRMDeviceType crmDeviceType, CRMDeviceID crmDeviceID )
	{
		CRMRec crmRec;
		crmRec.crmDeviceType = crmDeviceType;
		crmRec.crmDeviceID   = crmDeviceID;
		
		return CRMSearch( crmRec );
	}
	
	Owned< CRMRecPtr > New_CRMRecord()
	{
		Owned< CRMRecPtr > result = Owned< CRMRecPtr >::Seize
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
	
#endif  // #if CALL_NOT_IN_CARBON
	
	void RegisterCommunicationResourceManagerErrors()
	{
		// FIXME
	}
	
}

