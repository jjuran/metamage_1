#ifndef LAMP_PARAMETERBLOCK_H
#define LAMP_PARAMETERBLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

struct _lamp_user_parameter_block
{
	const void* stack_bottom;
};

typedef struct _lamp_user_parameter_block _lamp_user_parameter_block;

#ifdef __cplusplus
}
#endif

#endif

