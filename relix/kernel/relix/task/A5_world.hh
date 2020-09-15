/*
	A5_world.hh
	-----------
*/

#ifndef RELIX_TASK_A5WORLD_HH
#define RELIX_TASK_A5WORLD_HH


struct GrafPort;


namespace relix
{
	
	typedef GrafPort** a5_world_t;
	
#ifdef __MC68K__
	
	extern a5_world_t the_saved_A5_world;
	
	static inline
	asm a5_world_t* get_A5()
	{
		MOVEA.L  A5,A0
	}
	
	static inline
	void save_the_A5_world()
	{
		the_saved_A5_world = *get_A5();
	}
	
	static inline
	void restore_the_A5_world()
	{
		*get_A5() = the_saved_A5_world;
	}
	
	static inline
	a5_world_t swap_A5_worlds()
	{
		a5_world_t result = *get_A5();
		
		*get_A5() = the_saved_A5_world;
		
		return result;
	}
	
	static inline
	void load_A5_world( a5_world_t new_a5_world )
	{
		*get_A5() = new_a5_world;
	}
	
#else
	
	static inline
	void save_the_A5_world()
	{
	}
	
	static inline
	void restore_the_A5_world()
	{
	}
	
	static inline
	a5_world_t swap_A5_worlds()
	{
		return 0;  // NULL
	}
	
	static inline
	void load_A5_world( a5_world_t new_a5_world )
	{
	}
	
#endif
	
}

#endif
