#define COPY "\0xA9"

#define COPYRIGHT  "2026 Josh Juran"

#define VERSION  "2.0"

#define MAJOR  2
#define MINOR  0
#define POINT  0
#define STAGE  release
#define PATCH  0

resource 'vers' (1)
{
	MAJOR,
	MINOR << 4 | POINT,
	STAGE,
	PATCH,
	smRoman,
	VERSION,
	"Legacynth 4000 " VERSION ", " COPY COPYRIGHT
};
