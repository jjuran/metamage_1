/*	=======================
 *	FSTree_sys_mac_drive.cc
 *	=======================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_drive.hh"

#ifndef __DISKS__
#include <Disks.h>
#endif

// POSeven
#include "POSeven/Errno.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FileSystem/Drives.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_Stamp_Action.hh"
#include "Genie/FileSystem/FSTree_Virtual_Link.hh"
#include "Genie/FileSystem/ResolvePathname.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static const DrvQEl* FindDrive( UInt16 driveNumber )
	{
		N::DriveQueue_Container drives = N::DriveQueue();
		
		typedef N::DriveQueue_Container::const_iterator Iter;
		
		for ( Iter it = drives.begin();  it != drives.end();  ++it )
		{
			if ( it->dQDrive == driveNumber )
			{
				return &*it;
			}
		}
		
		return NULL;
	}
	
	std::string DriveNumber_KeyName_Traits::NameFromKey( const Key& key )
	{
		return Int_KeyName_Traits::NameFromKey( key );
	}
	
	DriveNumber_KeyName_Traits::Key DriveNumber_KeyName_Traits::KeyFromName( const std::string& name )
	{
		int n = Int_KeyName_Traits::KeyFromName( name );
		
		if ( n <= 0  ||  SInt16( n ) != n )
		{
			p7::throw_errno( ENOENT );
		}
		
		N::FSVolumeRefNum driveNum = N::FSVolumeRefNum( n );
		
		return driveNum;
	}
	
	bool DriveNumber_KeyName_Traits::KeyIsValid( const Key& key )
	{
		return FindDrive( key ) != NULL;
	}
	
	
	extern const FSTree_Premapped::Mapping sys_mac_drive_N_Mappings[];
	
	FSTreePtr sys_mac_drive_Details::GetChildNode( const FSTreePtr&    parent,
		                                           const std::string&  name,
		                                           const Key&          key )
	{
		return Premapped_Factory< sys_mac_drive_N_Mappings >( parent, name );
	}
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		return N::FSVolumeRefNum( std::atoi( parent->Name().c_str() ) );
	}
	
	static N::FSVolumeRefNum GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	static FSTreePtr Link_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		DriveNumber_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		const DrvQEl* el = FindDrive( key );
		
		if ( el == NULL )
		{
			return FSNull();
		}
		
		N::DriverRefNum refNum = N::DriverRefNum( el->dQRefNum );
		
		UnitNumber unit = ~refNum;
		
		std::string unitNumber = NN::Convert< std::string >( unit );
		
		std::string target = "/sys/mac/unit/" + unitNumber;
		
		return FSTreePtr( new FSTree_Virtual_Link( parent, name, target ) );
	}
	
	struct GetDriveFlags
	{
		typedef std::string Result;
		
		static std::string Get( const DrvQEl& drive )
		{
			const std::size_t sizeof_flags = 4;
			
			const UInt8* end   = (UInt8*) &drive;
			const UInt8* begin = end - sizeof_flags;
			
			using BitsAndBytes::EncodeAsHex;
			
			return EncodeAsHex( begin, sizeof_flags );
		}
	};
	
	struct GetDriveSize
	{
		typedef UInt32 Result;
		
		static UInt32 Get( const DrvQEl& drive )
		{
			UInt32 size = drive.dQDrvSz;
			
			if ( drive.qType != 0 )
			{
				size += drive.dQDrvSz2 << 16;
			}
			
			return size;
		}
	};
	
	struct GetDriveFSID
	{
		typedef SInt16 Result;
		
		static SInt16 Get( const DrvQEl& drive )
		{
			return drive.dQFSID;
		}
	};
	
	template < class Accessor >
	struct sys_mac_drive_N_Property
	{
		typedef N::FSVolumeRefNum Key;
		
		static std::string Read( const FSTree* that )
		{
			Key key = GetKey( that );
			
			const DrvQEl* el = FindDrive( key );
			
			if ( el == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			return NN::Convert< std::string >( Accessor::Get( *el ) );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_drive_N_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	template < class Stamp >
	static FSTreePtr Stamp_Factory( const FSTreePtr&    parent,
	                                const std::string&  name )
	{
		DriveNumber_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		return FSTreePtr( new Stamp( parent, name, key ) );
	}
	
	const FSTree_Premapped::Mapping sys_mac_drive_N_Mappings[] =
	{
		{ "driver", &Link_Factory },
		
		{ "flags", &Property_Factory< GetDriveFlags > },
		{ "fsid",  &Property_Factory< GetDriveFSID  > },
		{ "size",  &Property_Factory< GetDriveSize  > },
		
		{ "flush",  &Stamp_Factory< FSTree_Stamp_Action< Volume_Flush   > > },
		{ "umount", &Stamp_Factory< FSTree_Stamp_Action< Volume_Unmount > > },
		{ "eject",  &Stamp_Factory< FSTree_Stamp_Action< Volume_Eject   > > },
		{ "mount",  &Stamp_Factory< FSTree_Stamp_Action< Volume_Mount   > > },
		
		{ NULL, NULL }
	};
	
}

#endif

