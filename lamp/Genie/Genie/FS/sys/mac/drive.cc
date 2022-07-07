/*
	Genie/FS/sys/mac/drive.cc
	-------------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/drive.hh"

// Standard C++
#include <algorithm>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/serialize.hh"
#include "plus/string/concat.hh"

// poseven
#include "poseven/types/errno_t.hh"

// ClassicToolbox
#include "ClassicToolbox/Files.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/basic_directory.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"
#include "vfs/node/types/symbolic_link.hh"

// Genie
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
		
		return well_formed_name::applies( name )  &&  FindDrive( gear::parse_unsigned_decimal( name.c_str() ) ) != NULL;
	}
	
	
	extern const vfs::fixed_mapping sys_mac_drive_N_Mappings[];
	
	static vfs::node_ptr drive_lookup( const vfs::node* parent, const plus::string& name )
	{
		if ( !is_valid_drive_name( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name, sys_mac_drive_N_Mappings );
	}
	
	class drive_IteratorConverter
	{
		public:
			vfs::dir_entry operator()( const N::DriveQueue_Sequence::value_type& value ) const
			{
				const ino_t inode = value.dQDrive;
				
				plus::string name = gear::inscribe_decimal( value.dQDrive );
				
				return vfs::dir_entry( inode, name );
			}
	};
	
	static void drive_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		drive_IteratorConverter converter;
		
		N::DriveQueue_Sequence sequence = N::DriveQueue();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const vfs::node& parent )
	{
		return N::FSVolumeRefNum( gear::parse_decimal( parent.name().c_str() ) );
	}
	
	static vfs::node_ptr Link_Factory( const vfs::node*     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		const N::FSVolumeRefNum key = GetKeyFromParent( *parent );
		
		const DrvQEl* el = FindDrive( key );
		
		if ( el == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		N::DriverRefNum refNum = N::DriverRefNum( el->dQRefNum );
		
		UnitNumber unit = ~refNum;
		
		plus::string unitNumber = gear::inscribe_decimal( unit );
		
		plus::string target = "/sys/mac/unit/" + unitNumber;
		
		return vfs::new_symbolic_link( parent, name, target );
	}
	
	struct GetDriveFlags : plus::serialize_hex< UInt32 >
	{
		static UInt32 Get( const DrvQEl& drive )
		{
			return ((const UInt32*) &drive)[-1];
		}
	};
	
	struct GetDriveSize : plus::serialize_unsigned< UInt32 >
	{
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
	
	struct GetDriveFSID : plus::serialize_int< SInt16 >
	{
		static SInt16 Get( const DrvQEl& drive )
		{
			return drive.dQFSID;
		}
	};
	
	static const DrvQEl& FindDrive( const vfs::node& that )
	{
		N::FSVolumeRefNum vRefNum = GetKeyFromParent( that );
		
		const DrvQEl* el = FindDrive( vRefNum );
		
		if ( el == NULL )
		{
			throw vfs::undefined_property();
		}
		
		return *el;
	}
	
	template < class Accessor >
	struct sys_mac_drive_N_Property : vfs::readonly_property
	{
		static const int fixed_size = Accessor::fixed_size;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			const DrvQEl& el = FindDrive( *that );
			
			const typename Accessor::result_type data = Accessor::Get( el );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< sys_mac_drive_N_Property< prop > >::value
	
	const vfs::fixed_mapping sys_mac_drive_N_Mappings[] =
	{
		{ "driver", &Link_Factory },
		
		{ "flags", PROPERTY( GetDriveFlags ) },
		{ "fsid",  PROPERTY( GetDriveFSID  ) },
		{ "size",  PROPERTY( GetDriveSize  ) },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_sys_mac_drive( const vfs::node*     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		return vfs::new_basic_directory( parent, name, drive_lookup, drive_iterate );
	}
	
}

#endif
