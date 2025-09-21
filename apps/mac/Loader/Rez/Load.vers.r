#define COPYRIGHT  "2020-2025 Josh Juran"

#define COPY "\0xA9"

#define MAJOR 0
#define MINOR 1
#define POINT 0

#define VERSION  "0.1.0"

type 'Load' as 'STR ';

resource 'Load' (0, "Loader")
{
	"Copyright " COPY " " COPYRIGHT
};

resource 'vers' (1) {
	MAJOR,
	MINOR << 4 | POINT,
	release,
	0,
	smRoman,
	VERSION,
	VERSION ", " COPY COPYRIGHT
};
