/*
	recording.hh
	------------
*/

#ifndef RECORD_RECORDING_HH
#define RECORD_RECORDING_HH


struct iovec;

namespace v68k   {
namespace record {

extern int recording_fd;

int append( const void* data, unsigned size );

int appendv( const iovec* iov, unsigned n_iov );

inline
bool recording()
{
	return recording_fd >= 0;
}

}  // namespace record
}  // namespace v68k

#endif
