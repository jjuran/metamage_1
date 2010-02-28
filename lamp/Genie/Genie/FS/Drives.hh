/*	=========
 *	Drives.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_DRIVES_HH
#define GENIE_FILESYSTEM_DRIVES_HH

// ClassicToolbox
#include "ClassicToolbox/Files.hh"


namespace Genie
{
	
	template < void (*f)(Nitrogen::FSVolumeRefNum) >
	class Volume_Action
	{
		private:
			typedef Nitrogen::FSVolumeRefNum Key;
			
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
	
	typedef Volume_Action< Nitrogen::FlushVol   > Volume_Flush;
	typedef Volume_Action< Nitrogen::UnmountVol > Volume_Unmount;
	
#if !TARGET_API_MAC_CARBON
	
	typedef Volume_Action< Nitrogen::Eject    > Volume_Eject;
	typedef Volume_Action< Nitrogen::MountVol > Volume_Mount;
	
#endif
	
}

#endif

