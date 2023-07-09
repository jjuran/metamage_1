#ifndef RELIX_PARAMETERBLOCK_H
#define RELIX_PARAMETERBLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	runctl_get_max,
	runctl_allocate_syscall_stack,
	
	runctl_end
};

struct _relix_user_parameter_block
{
	const void* stack_bottom;
	const void* stack_limit;
	
	int* errno_var;
	
	void (*cleanup)();
	
#ifdef __MC68K__
	
	void* globals;
	void* a5_world;
	
#endif
};

typedef struct _relix_user_parameter_block _relix_user_parameter_block;


struct _relix_system_parameter_block
{
	_relix_user_parameter_block* current_user;
	
	const unsigned long system_block_length;  // size_t
	const unsigned long user_block_length;    // size_t
	
	void (*const dispatcher)( ... );
	
	unsigned long long (*const microsecond_timer)();  // uint64_t
	
	long (*const runctl)( long selector );
};

typedef struct _relix_system_parameter_block _relix_system_parameter_block;

#ifdef __cplusplus
}
#endif

#endif
