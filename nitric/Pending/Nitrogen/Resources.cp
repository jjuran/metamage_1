// Resources.cp

#ifndef NITROGEN_RESOURCES_H
#include "Nitrogen/Resources.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	Handle CheckResource( Handle h )
	{
		if ( h == NULL )
		{
			ResError();
			throw ResNotFound();
		}
		return h;
	}
	
	void CloseResFile( Owned< ResFileRefNum > resFileRefNum )
	{
		::CloseResFile( resFileRefNum.Release() );
		
		ResError();
	}
	
	void ResError()
	{
		OnlyOnce< RegisterResourceManagerErrors >();
		ThrowOSStatus( ::ResError() );
	}
	
	ResFileRefNum CurResFile()
	{
		ResFileRefNum refNum( ::CurResFile() );
		ResError();
		return refNum;
	}
	
	void UseResFile( ResFileRefNum resFileRefNum )
	{
		::UseResFile(resFileRefNum);
		ResError();
	}
	
	
	short CountTypes()
	{
		short count = ::CountTypes();
		ResError();
		
		return count;
	}
	
	short Count1Types()
	{
		short count = ::Count1Types();
		ResError();
		return count;
	}
	
	ResType GetIndType( short index )
	{
		::ResType type;
		::GetIndType( &type, index );
		ResError();
		
		return ResType( type );
	}
	
	ResType Get1IndType( short index )
	{
		::ResType type;
		::Get1IndType( &type, index );
		ResError();
		
		return ResType( type );
	}
	
	short CountResources( ResType type )
	{
		short count = ::CountResources( type );
		ResError();
		
		return count;
	}
	
	short Count1Resources( ResType type )
	{
		short count = ::Count1Resources( type );
		ResError();
		
		return count;
	}
	
	Handle GetIndResource( ResType type, short index )
	{
		return Handle( CheckResource( ::GetIndResource( type, index ) ) );
	}
	
	Handle Get1IndResource( ResType type, short index )
	{
		return Handle( CheckResource( ::Get1IndResource( type, index ) ) );
	}
	
	Handle GetResource( ResType type, ResID resID )
	{
		return Handle( CheckResource( ::GetResource( type, resID ) ) );
	}
	
	Handle Get1Resource( ResType type, ResID resID )
	{
		return Handle( CheckResource( ::Get1Resource( type, resID ) ) );
	}
	
	GetResInfo_Result GetResInfo( Handle h )
	{
		GetResInfo_Result result;
		::ResID id;
		::ResType type;
		
		::GetResInfo( h, &id, &type, result.name );
		ResError();
		
		result.id   = ResID  ( id   );
		result.type = ResType( type );
		return result;
	}
	
	Owned< Handle > DetachResource(Handle h)
	{
		::DetachResource( h );
		ResError();
		
		return Owned< Handle >::Seize( h );
	}
	
	Handle AddResource( Owned< Handle > h, ResType type, ResID resID, ConstStr255Param name )
	{
		::AddResource( h.Get(), type, resID, name );
		ResError();
		
		return h.Release();
	}
	
	Handle AddResource( Owned< Handle > h, const GetResInfo_Result& resInfo )
	{
		return AddResource( h, resInfo.type, resInfo.id, resInfo.name );
	}
	
	Owned< ResFileRefNum > FSpOpenResFile( const FSSpec& spec, FSIOPermssn permissions )
	{
		ResFileRefNum refNum( ::FSpOpenResFile( &spec, permissions ) );
		
		ResError();
		
		return Owned< ResFileRefNum >::Seize( refNum );
	}
	
	Owned< ResFileRefNum > FSOpenResourceFile( const FSRef&    ref, 
	                                           UniCharCount    forkNameLength, 
	                                           const UniChar*  forkName, 
	                                           FSIOPermssn     permissions )
	{
		::ResFileRefNum refNum;
		ThrowOSStatus( ::FSOpenResourceFile( &ref, forkNameLength, forkName, permissions, &refNum ) );
		return Owned< ResFileRefNum >::Seize( ResFileRefNum( refNum ) );
	}
	
	Owned< ResFileRefNum > FSOpenResourceFile( const FSRef&      ref, 
	                                           const UniString&  forkName, 
	                                           FSIOPermssn       permissions )
	{
		return FSOpenResourceFile( ref, forkName.size(), forkName.data(), permissions );
	}
	
	void RegisterResourceManagerErrors()
	{
		RegisterOSStatus< dirFulErr        >();
		RegisterOSStatus< dskFulErr        >();
		RegisterOSStatus< bdNamErr         >();
		RegisterOSStatus< ioErr            >();
		RegisterOSStatus< bdNamErr         >();
		RegisterOSStatus< eofErr           >();
		RegisterOSStatus< tmfoErr          >();
		RegisterOSStatus< fnfErr           >();
		RegisterOSStatus< wPrErr           >();
		RegisterOSStatus< fLckdErr         >();
		RegisterOSStatus< vLckdErr         >();
		RegisterOSStatus< dupFNErr         >();
		RegisterOSStatus< opWrErr          >();
		RegisterOSStatus< permErr          >();
		RegisterOSStatus< extFSErr         >();
		RegisterOSStatus< memFullErr       >();
		RegisterOSStatus< dirNFErr         >();
		RegisterOSStatus< resourceInMemory >();
		RegisterOSStatus< writingPastEnd   >();
		RegisterOSStatus< inputOutOfBounds >();
		RegisterOSStatus< resNotFound      >();
		RegisterOSStatus< resFNotFound     >();
		RegisterOSStatus< addResFailed     >();
		RegisterOSStatus< rmvResFailed     >();
		RegisterOSStatus< resAttrErr       >();
		RegisterOSStatus< mapReadErr       >();
	}
	
}

