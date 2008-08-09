/*	=======================
 *	FSTree_sys_mac_drive.hh
 *	=======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACDRIVE_HH
#define GENIE_FILESYSTEM_FSTREESYSMACDRIVE_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Nitrogen
{
	
	class DriveQueue_Container
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
				friend class DriveQueue_Container;
				
				public:
					typedef DriveQueue_Container::size_type  size_type;
					typedef DriveQueue_Container::reference  reference;
					typedef DriveQueue_Container::pointer    pointer;
					
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
	
	inline DriveQueue_Container DriveQueue()
	{
		return DriveQueue_Container();
	}
	
}


namespace Genie
{
	
	struct DriveNumber_KeyName_Traits
	{
		typedef Nitrogen::FSVolumeRefNum Key;
		
		static std::string NameFromKey( const Key& key );
		
		static Key KeyFromName( const std::string& name );
		
		static bool KeyIsValid( const Key& key );
	};
	
	
	struct sys_mac_drive_Details : public DriveNumber_KeyName_Traits
	{
		typedef Nitrogen::DriveQueue_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::DriveQueue(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return Key( value.dQDrive ); }
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_drive_Details > FSTree_sys_mac_drive;
	
}

#endif

