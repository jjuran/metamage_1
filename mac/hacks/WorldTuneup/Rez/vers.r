#include "version.h"

#define COPY "\0xA9"

resource 'vers' (1)
{
	MAJOR,
	MINOR << 4 | POINT,
	STAGE,
	PATCH,
	smRoman,
	VERSION,
	VERSION ", " COPY COPYRIGHT
};
