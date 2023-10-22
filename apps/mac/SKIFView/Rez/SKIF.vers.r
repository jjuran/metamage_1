#define COPYRIGHT  "2021-2023 Josh Juran"

#define COPY "\0xA9"

type 'SKIF' as 'STR ';

resource 'SKIF' (0, "SKIFView")
{
	"Copyright " COPY " " COPYRIGHT
};

resource 'vers' (1) {
	0x00,
	0x20,
	release,
	0,
	smRoman,
	"0.2",
	"0.2, " COPY COPYRIGHT
};
