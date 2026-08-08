#include "app_name.h"
#include "version.h"

#define COPY "\0xA9"

type CREATOR as 'STR ';

resource CREATOR (0, APP_NAME)
{
	"Copyright " COPY " " COPYRIGHT
};

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
