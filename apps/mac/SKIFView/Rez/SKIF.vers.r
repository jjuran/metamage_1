#define COPYRIGHT  "2021 Josh Juran"

#define COPY "\0xA9"

type 'SKIF' as 'STR ';

resource 'SKIF' (0, "SKIFView")
{
	"Copyright " COPY " " COPYRIGHT
};

resource 'vers' (1) {
	0x00,
	0x11,
	release,
	0,
	smRoman,
	"0.1.1",
	"0.1.1, " COPY COPYRIGHT
};
