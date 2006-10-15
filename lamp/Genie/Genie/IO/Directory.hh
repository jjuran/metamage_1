/*	============
 *	Directory.hh
 *	============
 */

#ifndef GENIE_IO_DIRECTORY_HH
#define GENIE_IO_DIRECTORY_HH

// POSIX
#include "dirent.h"
#include "sys/types.h"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	class DirHandle : public IOHandle
	{
		private:
			static const SInt32 kStartIndex = -2;
		
		protected:
			dirent fLastEntry;
			SInt32 fIndex;
		
		private:
			N::FSDirSpec fDir;
		
		public:
			DirHandle( const N::FSDirSpec& dir )
			:
				fIndex( kStartIndex ),
				fDir( dir )
			{}
			
			virtual ~DirHandle()  {}
			
			const N::FSDirSpec& GetDir() const  { return fDir; }
			
			virtual const dirent* ReadDir();
			
			void RewindDir();
			void SeekDir( off_t index );
			off_t TellDir() const;
			
	};
	
	class VolumesDirHandle : public DirHandle
	{
		public:
			VolumesDirHandle() : DirHandle( N::FSDirSpec() )  {}
			
			const dirent* ReadDir();
	};
	
	class ProcDirHandle : public DirHandle
	{
		public:
			ProcDirHandle() : DirHandle( N::FSDirSpec() )  {}
			
			const dirent* ReadDir();
	};
	
}

#endif

