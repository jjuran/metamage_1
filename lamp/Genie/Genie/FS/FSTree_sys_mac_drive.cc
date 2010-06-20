/*	=======================
 *	FSTree_sys_mac_drive.cc
 *	=======================
 */

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/FSTree_sys_mac_drive.hh"

// Iota
#include "iota/decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// ClassicToolbox
#include "ClassicToolbox/Files.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/Drives.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Null.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/stringify.hh"
#include "Genie/FS/SymbolicLink.hh"
#include "Genie/FS/Trigger.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static const DrvQEl* FindDrive( UInt16 driveNumber )
	{
		const N::DriveQueue_Sequence drives = N::DriveQueue();
		
		typedef N::DriveQueue_Sequence::const_iterator Iter;
		
		for ( Iter it = drives.begin();  it != drives.end();  ++it )
		{
			if ( it->dQDrive == driveNumber )
			{
				return &*it;
			}
		}
		
		return NULL;
	}
	
	static bool is_valid_drive_name( const plus::string& name )
	{
		typedef canonical_positive_integer well_formed_name;
		
		return well_formed_name::applies( name )  &&  FindDrive( iota::parse_unsigned_decimal( name.c_str() ) ) != NULL;
	}
	
	
	extern const FSTree_Premapped::Mapping sys_mac_drive_N_Mappings[];
	
	static FSTreePtr drive_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !is_valid_drive_name( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory< sys_mac_drive_N_Mappings >( parent, name, NULL );
	}
	
	class drive_IteratorConverter
	{
		public:
			FSNode operator()( const N::DriveQueue_Sequence::value_type& value ) const
			{
				const ino_t inode = value.dQDrive;
				
				plus::string name = iota::inscribe_decimal( value.dQDrive );
				
				return FSNode( inode, name );
			}
	};
	
	static void drive_iterate( FSTreeCache& cache )
	{
		drive_IteratorConverter converter;
		
		N::DriveQueue_Sequence sequence = N::DriveQueue();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		return N::FSVolumeRefNum( iota::parse_decimal( parent->Name().c_str() ) );
	}
	
	static N::FSVolumeRefNum GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	static FSTreePtr Link_Factory( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		const N::FSVolumeRefNum key = GetKeyFromParent( parent );
		
		const DrvQEl* el = FindDrive( key );
		
		if ( el == NULL )
		{
			return FSNull();
		}
		
		N::DriverRefNum refNum = N::DriverRefNum( el->dQRefNum );
		
		UnitNumber unit = ~refNum;
		
		plus::string unitNumber = iota::inscribe_decimal( unit );
		
		plus::string target = "/sys/mac/unit/" + unitNumber;
		
		return New_FSTree_SymbolicLink( parent, name, target );
	}
	
	struct GetDriveFlags
	{
		typedef UInt32 Result;
		
		typedef stringify_32_bit_hex stringify;
		
		static Result Get( const DrvQEl& drive )
		{
			return ((const Result*) &drive)[-1];
		}
	};
	
	struct GetDriveSize
	{
		typedef UInt32 Result;
		
		typedef stringify_unsigned stringify;
		
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
		
		typedef stringify_short stringify;
		
		static SInt16 Get( const DrvQEl& drive )
		{
			return drive.dQFSID;
		}
	};
	
	static const DrvQEl& FindDrive( const FSTree* that )
	{
		N::FSVolumeRefNum vRefNum = GetKey( that );
		
		const DrvQEl* el = FindDrive( vRefNum );
		
		if ( el == NULL )
		{
			throw FSTree_Property::Undefined();
		}
		
		return *el;
	}
	
	template < class Accessor >
	struct sys_mac_drive_N_Property
	{
		static plus::string Read( const FSTree* that, bool binary )
		{
			const DrvQEl& el = FindDrive( that );
			
			const typename Accessor::Result data = Accessor::Get( el );
			
			plus::string result = Accessor::stringify::apply( data, binary );
			
			return result;
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		typedef sys_mac_drive_N_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	template < class Trigger >
	static FSTreePtr Trigger_Factory( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		const N::FSVolumeRefNum key = GetKeyFromParent( parent );
		
		return seize_ptr( new Trigger( parent, name, key ) );
	}
	
	const FSTree_Premapped::Mapping sys_mac_drive_N_Mappings[] =
	{
		{ "driver", &Link_Factory },
		
		{ "flags", &Property_Factory< GetDriveFlags > },
		{ "fsid",  &Property_Factory< GetDriveFSID  > },
		{ "size",  &Property_Factory< GetDriveSize  > },
		
		{ "flush",  &Trigger_Factory< Trigger< Volume_Flush   > > },
		{ "umount", &Trigger_Factory< Trigger< Volume_Unmount > > },
		{ "eject",  &Trigger_Factory< Trigger< Volume_Eject   > > },
		{ "mount",  &Trigger_Factory< Trigger< Volume_Mount   > > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_drive( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		return new_basic_directory( parent, name, drive_lookup, drive_iterate );
	}
	
}

#endif

