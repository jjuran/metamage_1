/*	=========
 *	Drives.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_DRIVES_HH
#define GENIE_FILESYSTEM_DRIVES_HH

// Nitrogen
#ifndef MAC_FILES_FUNCTIONS_FLUSHVOL_HH
#include "Mac/Files/Functions/FlushVol.hh"
#endif
#ifndef MAC_FILES_FUNCTIONS_UNMOUNTVOL_HH
#include "Mac/Files/Functions/UnmountVol.hh"
#endif

// ClassicToolbox
#if !TARGET_API_MAC_CARBON
#ifndef CLASSICTOOLBOX_FILES_HH
#include "ClassicToolbox/Files.hh"
#endif
#endif


namespace Genie
{
	
	class FSTree;
	
	void volume_flush_trigger( const FSTree* node );
	void volume_eject_trigger( const FSTree* node );
	void volume_mount_trigger( const FSTree* node );
	void volume_unmount_trigger( const FSTree* node );
	
	template < void (*f)(Mac::FSVolumeRefNum) >
	class Volume_Action
	{
		private:
			typedef Mac::FSVolumeRefNum Key;
			
			Key itsKey;
		
		public:
			Volume_Action( Key key ) : itsKey( key )
			{
			}
			
			void operator()() const
			{
				f( itsKey );
			}
	};
	
	typedef Volume_Action< Mac::FlushVol   > Volume_Flush;
	typedef Volume_Action< Mac::UnmountVol > Volume_Unmount;
	
#if !TARGET_API_MAC_CARBON
	
	typedef Volume_Action< Nitrogen::Eject    > Volume_Eject;
	typedef Volume_Action< Nitrogen::MountVol > Volume_Mount;
	
#endif
	
}

#endif

