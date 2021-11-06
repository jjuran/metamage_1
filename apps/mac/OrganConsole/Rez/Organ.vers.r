#define COPYRIGHT  "2021-2023 Josh Juran"

#define COPY "\0xA9"

#define MAJOR 0
#define MINOR 5
#define TWEAK 0

#define VERSION  "0.5"

type '|gan' as 'STR ';

resource '|gan' (0, "Organ Console")
{
	"Copyright " COPY " " COPYRIGHT
};

resource 'vers' (1) {
	MAJOR,
	MINOR << 4 | TWEAK,
	release,
	0,
	smRoman,
	VERSION,
	VERSION ", " COPY COPYRIGHT
};
