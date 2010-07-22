#ifndef LAMP_PARAMETERBLOCK_H
#define LAMP_PARAMETERBLOCK_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _lamp_user_parameter_block
{
	const void* stack_bottom;
	const void* stack_limit;
	
	int* errno_var;
	
	void (*cleanup)();
};

typedef struct _lamp_user_parameter_block _lamp_user_parameter_block;


struct _lamp_system_parameter_block
{
	_lamp_user_parameter_block* current_user;
	
	const size_t system_block_length;
	const size_t user_block_length;
	
	void (*const dispatcher)( ... );
	
	uint64_t (*const microsecond_timer)();
};

typedef struct _lamp_system_parameter_block _lamp_system_parameter_block;

#ifdef __cplusplus
}
#endif

#endif

