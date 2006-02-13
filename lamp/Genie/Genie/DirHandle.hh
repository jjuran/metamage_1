/*	============
 *	DirHandle.hh
 *	============
 */

#ifndef GENIE_DIRHANDLE_HH
#define GENIE_DIRHANDLE_HH

// POSIX
#include "dirent.h"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/IORef.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	IORef OpenDir( const N::FSDirSpec& dir );
	
	class DirHandle : public IOStream
	{
		protected:
			dirent fLastEntry;
			UInt32 fIndex;
		
		private:
			N::FSDirSpec fDir;
		
		public:
			DirHandle( const N::FSDirSpec& dir )
			:
				fIndex( 0 ),
				fDir( dir )
			{}
			
			virtual ~DirHandle()  {}
			
			const N::FSDirSpec& GetDir() const  { return fDir; }
			
			virtual const dirent* ReadDir();
			
			void RewindDir();
			void SeekDir( off_t index );
			off_t TellDir() const;
			
			unsigned int SysPoll() const  { return kPollRead | kPollExcept; }
			
			int SysRead( char* data, std::size_t byteCount )  { throw N::ParamErr(); }
			
			int SysWrite( const char* data, std::size_t byteCount )  { throw N::ParamErr(); }
			
			// Synchronous filesystem I/O is always blocking
			bool IsBlocking() const  { return true; }
			
			void SetBlocking()  {}
			
			void SetNonBlocking()  { throw N::ParamErr(); }
	};
	
}

#endif

