#define COPYRIGHT  "2017-2023 Josh Juran"

#define COPY "\0xA9"

type 'NYAN' as 'STR ';

resource 'NYAN' (0, "Nyanochrome Cat")
{
	"Copyright " COPY " " COPYRIGHT
};

resource 'vers' (1) {
	0x01,
	0x10,
	release,
	0,
	smRoman,
	"1.1",
	"1.1, " COPY " " COPYRIGHT
};

resource 'STR#' (128) {{
	"About",
	"(With apologies to Chris Torres)",
}};
