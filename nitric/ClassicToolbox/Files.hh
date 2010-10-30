/*	========
 *	Files.hh
 *	========
 */

#ifndef CLASSICTOOLBOX_FILES_HH
#define CLASSICTOOLBOX_FILES_HH

#ifndef __DISKS__
#include <Disks.h>
#endif

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif


namespace Nitrogen
{
	
	void Eject( ConstStr63Param volName );
	
	void Eject( FSVolumeRefNum vRefNum );
	
	void MountVol( FSVolumeRefNum driveNumber );
	
#if !TARGET_API_MAC_CARBON
	
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
	
#endif
	
}

#endif

