// Nitrogen/Resources.cc
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Resources.hh"

// Nitrogen
#include "Nitrogen/MacErrors.hh"
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( ResourceManager )
	
	
	static void RegisterResourceManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class ResourceManagerErrorsRegistration
	{
		public:
			ResourceManagerErrorsRegistration()  { RegisterResourceManagerErrors(); }
	};
	
	static ResourceManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	void CloseResFile( nucleus::owned< ResFileRefNum > resFileRefNum )
	{
		::CloseResFile( resFileRefNum.release() );
		
		ResError();
	}
	
	ResFileRefNum CurResFile()
	{
		ResFileRefNum refNum = ResFileRefNum( ::CurResFile() );
		
		return refNum;
	}
	
	ResFileRefNum HomeResFile( Handle r )
	{
		ResFileRefNum refNum = ResFileRefNum( ::HomeResFile( r ) );
		ResError();
		
		return refNum;
	}
	
	void UseResFile( ResFileRefNum resFileRefNum )
	{
		::UseResFile( resFileRefNum );
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
	
	Handle GetNamedResource( ResType type, ConstStr255Param name )
	{
		return Handle( CheckResource( ::GetNamedResource( type, name ) ) );
	}
	
	Handle Get1NamedResource( ResType type, ConstStr255Param name )
	{
		return Handle( CheckResource( ::Get1NamedResource( type, name ) ) );
	}
	
	void MacLoadResource( Handle r )
	{
		::MacLoadResource( r );
		ResError();
	}
	
	void ReleaseResource( Handle r )
	{
		::ReleaseResource( r );
		ResError();
	}
	
	nucleus::owned< Handle > DetachResource( Handle r )
	{
		::DetachResource( r );
		ResError();
		
		return nucleus::owned< Handle >::seize( r );
	}
	
	ResID UniqueID ( ResType type )
	{
		ResID resID = ResID( ::UniqueID( type ) );
		ResError();
		
		return resID;
	}
	
	ResID Unique1ID ( ResType type )
	{
		ResID resID = ResID( ::Unique1ID( type ) );
		ResError();
		
		return resID;
	}
	
	ResAttributes GetResAttrs( Handle r )
	{
		ResAttributes attrs = ResAttributes( ::GetResAttrs( r ) );
		ResError();
		
		return attrs;
	}
	
	GetResInfo_Result GetResInfo( Handle r )
	{
		GetResInfo_Result result;
		::ResID id;
		::ResType type;
		
		::GetResInfo( r, &id, &type, result.name );
		ResError();
		
		result.id   = ResID  ( id   );
		result.type = ResType( type );
		
		return result;
	}
	
	void SetResInfo( Handle r, ResID id, ConstStr255Param name )
	{
		::SetResInfo( r, id, name );
		ResError();
	}
	
	Handle AddResource( nucleus::owned< Handle >   h,
	                    ResType           type,
	                    ResID             resID,
	                    ConstStr255Param  name )
	{
		::AddResource( h.get(), type, resID, name );
		ResError();
		
		return h.release();
	}
	
	Handle AddResource( nucleus::owned< Handle > h, const GetResInfo_Result& resInfo )
	{
		return AddResource( h, resInfo.type, resInfo.id, resInfo.name );
	}
	
	std::size_t GetResourceSizeOnDisk( Handle r )
	{
		std::size_t size( ::GetResourceSizeOnDisk( r ) );
		ResError();
		
		return size;
	}
	
	std::size_t GetMaxResourceSize( Handle r )
	{
		std::size_t size( ::GetMaxResourceSize( r ) );
		ResError();
		
		return size;
	}
	
	void SetResAttrs( Handle r, ResAttributes attrs )
	{
		::SetResAttrs( r, attrs );
		ResError();
	}
	
	void ChangedResource( Handle r )
	{
		::ChangedResource( r );
		ResError();
	}
	
	nucleus::owned< Handle > RemoveResource( Handle r )
	{
		// New Inside Macintosh says that after calling RemoveResource() on a
		// resource handle, the memory will be released either by calling DisposeHandle,
		// or automatically when UpdateResFile (which is called by CloseResFile) is called.
		// Marshall informs me as of 2005-01-19 that the latter does not happen.
		// Therefore, the handle is a pure Memory Manager handle which must be disposed
		// by the application, and consequently is Owned.
		
		// <http://developer.apple.com/documentation/mac/MoreToolbox/MoreToolbox-87.html>
		
		::RemoveResource( r );
		ResError();
		
		return nucleus::owned< Handle >::seize( r );
	}
	
	void UpdateResFile( ResFileRefNum refNum )
	{
		::UpdateResFile( refNum );
		ResError();
	}
	
	void WriteResource( Handle r )
	{
		::WriteResource( r );
		ResError();
	}
	
	void SetResPurge( bool install )
	{
		::SetResPurge( install );
		ResError();
	}
	
	ResFileAttributes GetResFileAttrs( ResFileRefNum refNum )
	{
		ResFileAttributes attrs = ResFileAttributes( ::GetResFileAttrs( refNum ) );
		ResError();
		
		return attrs;
	}
	
	void SetResFileAttrs( ResFileRefNum refNum, ResFileAttributes attrs )
	{
		::SetResFileAttrs( refNum, attrs );
		ResError();
	}
	
	nucleus::owned< ResFileRefNum > FSpOpenResFile( const FSSpec& spec, Mac::FSIOPerm permissions )
	{
		ResFileRefNum refNum = ResFileRefNum( ::FSpOpenResFile( &spec, permissions ) );
		
		ResError();
		
		return nucleus::owned< ResFileRefNum >::seize( refNum );
	}
	
	void FSpCreateResFile( const FSSpec&  spec,
	                       OSType         creator,
	                       OSType         type,
	                       ScriptCode     scriptTag )
	{
		::FSpCreateResFile( &spec, creator, type, scriptTag );
		
		ResError();
	}
	
	nucleus::owned< ResFileRefNum > FSOpenResourceFile( const FSRef&    ref, 
	                                                    UniCharCount    forkNameLength, 
	                                                    const UniChar*  forkName, 
	                                                    Mac::FSIOPerm   permissions )
	{
		::ResFileRefNum refNum;
		ThrowOSStatus( ::FSOpenResourceFile( &ref,
		                                     forkNameLength,
		                                     forkName,
		                                     permissions,
		                                     &refNum ) );
		
		return nucleus::owned< ResFileRefNum >::seize( ResFileRefNum( refNum ) );
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

