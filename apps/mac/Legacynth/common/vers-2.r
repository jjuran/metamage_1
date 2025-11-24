#define COPY "\0xA9"

#define VERSION  "1.5.1"

#define MAJOR  1
#define MINOR  5
#define POINT  1
#define STAGE  release
#define PATCH  0

resource 'vers' (2)
{
	MAJOR,
	MINOR << 4 | POINT,
	STAGE,
	PATCH,
	smRoman,
	VERSION,
	"Legacynth " VERSION
};
