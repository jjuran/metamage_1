#define COPYRIGHT  "1999-2024 Josh Juran"

#define COPY "\0xA9"

#define MAJOR 0
#define MINOR 7
#define POINT 9
#define STAGE alpha
#define PATCH 2

#define VERSION  "0.7.9-alpha.2"

resource 'vers' (1) {
	MAJOR,
	MINOR << 4 | POINT,
	STAGE,
	PATCH,
	smRoman,
	VERSION,
	VERSION ", " COPY COPYRIGHT
};

/* Owner resource */

type 'Poof' as 'STR ';

resource 'Poof' (0, "MacRelix")
{
	"Copyright " COPY " " COPYRIGHT
};
