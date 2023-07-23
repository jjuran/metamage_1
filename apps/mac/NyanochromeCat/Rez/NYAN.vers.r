#define COPYRIGHT  "2017-2023 Josh Juran"

#define COPY "\0xA9"

type 'NYAN' as 'STR ';

resource 'NYAN' (0, "Nyanochrome Cat")
{
	"Copyright " COPY " " COPYRIGHT
};

resource 'vers' (1) {
	0x00,
	0x80,
	release,
	0,
	smRoman,
	"0.8.0",
	"0.8.0, " COPY " " COPYRIGHT
};
