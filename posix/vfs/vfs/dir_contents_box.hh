/*
	dir_contents_box.hh
	-------------------
*/

#ifndef VFS_DIRCONTENTSBOX_HH
#define VFS_DIRCONTENTSBOX_HH


namespace vfs
{
	
	class dir_contents;
	
	class dir_contents_box
	{
		private:
			dir_contents* its_contents;
		
		public:
			dir_contents_box() : its_contents()
			{
			}
			
			dir_contents_box( const dir_contents_box& that );
			
			dir_contents_box& operator=( const dir_contents_box& that );
			
			~dir_contents_box();
			
			void swap( dir_contents_box& that );
			
			dir_contents* get() const  { return its_contents; }
			
			dir_contents* get_new();
			
			dir_contents* operator->()  { return get_new(); }
			dir_contents& operator*()  { return *get_new(); }
	};
	
}

#endif

