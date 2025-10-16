/*
	data.hh
	-------
*/

#ifndef MACCG_DATA_HH
#define MACCG_DATA_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGIMAGE_H_
#ifndef __CGIMAGE__
#include <CGImage.h>
#endif
#endif

// Standard C
#include <stddef.h>


namespace mac {
namespace cg  {

CGDataProviderRef make_data_provider_xfer( void* data, size_t size );
CGDataProviderRef make_data_provider_copy( void* data, size_t size );

}
}

#endif
