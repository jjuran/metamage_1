/*
	listing.hh
	----------
*/

#ifndef MACFILE_LISTING_HH
#define MACFILE_LISTING_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


namespace mac  {
namespace file {
	
#if ! __LP64__
	
	struct list_entry
	{
		Str63       name;
		CInfoPBRec  cInfo;
	};
	
	typedef list_entry** list_handle;
	
	OSErr list_directory( Handle           result,
	                      short            vRefNum,
	                      long             dirID,
	                      ConstStr63Param  name = NULL );
	
	inline
	OSErr list_directory( list_handle      result,
	                      short            vRefNum,
	                      long             dirID,
	                      ConstStr63Param  name = NULL )
	{
		return list_directory( (Handle) result, vRefNum, dirID, name );
	}
	
	inline
	unsigned count_list_entries( Handle h )
	{
		return GetHandleSize( h ) / sizeof (list_entry);
	}
	
	inline
	unsigned count_list_entries( list_handle h )
	{
		return count_list_entries( (Handle) h );
	}
	
	inline
	unsigned count( list_handle h )
	{
		return count_list_entries( h );
	}
	
	inline
	list_entry& get_nth_list_entry( list_handle h, unsigned i )
	{
		return h[0][ i ];
	}
	
	inline
	list_entry& get_nth_list_entry( Handle h, unsigned i )
	{
		return get_nth_list_entry( (list_handle) h, i );
	}
	
	inline
	list_entry& get_nth( list_handle h, unsigned i )
	{
		return get_nth_list_entry( h, i );
	}
	
	class directory_listing
	{
		private:
			Handle its_handle;
			
			// non-copyable
			directory_listing           ( const directory_listing& );
			directory_listing& operator=( const directory_listing& );
		
		public:
			directory_listing() : its_handle( NewHandle( 0 ) )
			{
				/*
					In practice, the NewHandle() call shouldn't fail.
					In the event it does, we'll harmlessly pass NULL to
					DisposeHandle(), and presumably to SetHandleSize(),
					which will report the error and abort list_directory().
				*/
			}
			
			~directory_listing()
			{
				DisposeHandle( its_handle );
			}
			
			list_handle get() const  { return (list_handle) its_handle; }
			
			unsigned count() const
			{
				return count_list_entries( get() );
			}
			
			const list_entry& get_nth( unsigned i ) const
			{
				return get_nth_list_entry( get(), i );
			}
			
			list_entry& get_nth( unsigned i )
			{
				return get_nth_list_entry( get(), i );
			}
	};
	
	inline
	OSErr list_directory( directory_listing&  listing,
	                      short               vRefNum,
	                      long                dirID,
	                      ConstStr63Param     name = NULL )
	{
		return list_directory( listing.get(), vRefNum, dirID, name );
	}
	
#endif
	
}
}

#endif
