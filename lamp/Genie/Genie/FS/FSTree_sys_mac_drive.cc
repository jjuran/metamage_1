/*	=======================
 *	FSTree_sys_mac_drive.cc
 *	=======================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FS/FSTree_sys_mac_drive.hh"

#ifndef __DISKS__
#include <Disks.h>
#endif

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FS/Drives.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/FSTree_Virtual_Link.hh"
#include "Genie/FS/Trigger.hh"


namespace Nitrogen
{
	
	class DriveQueue_Sequence
	{
		public:
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			typedef DrvQEl value_type;
			
			typedef const value_type &reference;
			typedef const value_type &const_reference;
			
			typedef const value_type *pointer;
			typedef const value_type *const_pointer;
			
			class const_iterator
			{
				friend class DriveQueue_Sequence;
				
				public:
					typedef DriveQueue_Sequence::size_type  size_type;
					typedef DriveQueue_Sequence::reference  reference;
					typedef DriveQueue_Sequence::pointer    pointer;
					
					typedef std::forward_iterator_tag iterator_category;
					
				private:
					pointer  position;
					
					void Advance()
					{
						position = (pointer) position->qLink;
					}
					
					const_iterator( pointer pos ) : position( pos )
					{
					}
					
				public:
					const_iterator& operator++()      { Advance();  return *this; }
					const_iterator operator++(int)    { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const       { return *position; }
					pointer operator->() const        { return  position; }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			const_iterator begin() const              { return const_iterator( (pointer) GetDrvQHdr()->qHead ); }
			const_iterator end() const                { return const_iterator( NULL                          ); }
	};
	
	inline DriveQueue_Sequence DriveQueue()
	{
		return DriveQueue_Sequence();
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	struct DriveNumber_KeyName_Traits
	{
		typedef Nitrogen::FSVolumeRefNum Key;
		
		static std::string NameFromKey( const Key& key );
		
		static Key KeyFromName( const std::string& name );
		
		static bool KeyIsValid( const Key& key );
	};
	
	
	struct sys_mac_drive_Details : public DriveNumber_KeyName_Traits
	{
		typedef Nitrogen::DriveQueue_Sequence Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::DriveQueue(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return Key( value.dQDrive ); }
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_drive_Details > FSTree_sys_mac_drive;
	
	
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
		
		return New_FSTree_Virtual_Link( parent, name, target );
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
		
		static std::string Read( const FSTree* that, bool binary )
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
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	template < class Trigger >
	static FSTreePtr Trigger_Factory( const FSTreePtr&    parent,
	                                  const std::string&  name )
	{
		DriveNumber_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		return FSTreePtr( new Trigger( parent, name, key ) );
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
	
	FSTreePtr New_FSTree_sys_mac_drive( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_sys_mac_drive( parent, name ) );
	}
	
}

#endif

